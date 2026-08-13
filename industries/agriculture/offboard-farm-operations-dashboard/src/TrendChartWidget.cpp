// SPDX-License-Identifier: MIT
#include "TrendChartWidget.h"

#include "FarmIconPainter.h"

#include <QPainter>
#include <QPainterPath>

#include <algorithm>

namespace qttutorial::agriculture::ops {

TrendChartWidget::TrendChartWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(180);
}

void TrendChartWidget::setSeries(const std::vector<double>& values, double minValue, double maxValue,
                                  const QString& unit)
{
    m_values = values;
    m_minValue = minValue;
    m_maxValue = maxValue;
    m_unit = unit;
    m_hasData = !values.empty();
    update();
}

void TrendChartWidget::clear()
{
    m_values.clear();
    m_hasData = false;
    update();
}

void TrendChartWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#1c212b"));

    icons::paintFuelGlyph(painter, QRectF(8, 8, 16, 16), QColor("#e5b93d"));

    if (!m_hasData || m_values.size() < 2) {
        painter.setPen(QColor("#9aa4b2"));
        painter.drawText(rect(), Qt::AlignCenter, tr("Select a field to see its recent trend"));
        return;
    }

    const QRectF plotRect(32, 16, width() - 44, height() - 40);

    painter.setPen(QColor("#2a2f3a"));
    painter.drawRect(plotRect);

    const double range = std::max(1e-6, m_maxValue - m_minValue);
    const auto yFor = [&](double value) {
        const double fraction = std::clamp((value - m_minValue) / range, 0.0, 1.0);
        return plotRect.bottom() - fraction * plotRect.height();
    };

    QPainterPath linePath;
    QPainterPath fillPath;
    const double stepX = plotRect.width() / static_cast<double>(m_values.size() - 1);
    for (std::size_t i = 0; i < m_values.size(); ++i) {
        const double x = plotRect.left() + stepX * static_cast<double>(i);
        const double y = yFor(m_values[i]);
        if (i == 0) {
            linePath.moveTo(x, y);
            fillPath.moveTo(x, plotRect.bottom());
            fillPath.lineTo(x, y);
        } else {
            linePath.lineTo(x, y);
            fillPath.lineTo(x, y);
        }
    }
    fillPath.lineTo(plotRect.right(), plotRect.bottom());
    fillPath.closeSubpath();

    painter.setPen(Qt::NoPen);
    QColor fillColor("#3ddc6f");
    fillColor.setAlpha(40);
    painter.setBrush(fillColor);
    painter.drawPath(fillPath);

    painter.setPen(QPen(QColor("#3ddc6f"), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(linePath);

    painter.setPen(QColor("#9aa4b2"));
    painter.drawText(QRectF(plotRect.left(), 0, plotRect.width(), 16), Qt::AlignLeft,
                      tr("max %1%2").arg(m_maxValue).arg(m_unit));
    painter.drawText(QRectF(plotRect.left(), plotRect.bottom() + 2, plotRect.width(), 16), Qt::AlignLeft,
                      tr("min %1%2").arg(m_minValue).arg(m_unit));
    painter.drawText(QRectF(plotRect.left(), plotRect.bottom() + 2, plotRect.width(), 16), Qt::AlignRight,
                      tr("latest: %1%2").arg(m_values.back(), 0, 'f', 1).arg(m_unit));
}

} // namespace qttutorial::agriculture::ops
