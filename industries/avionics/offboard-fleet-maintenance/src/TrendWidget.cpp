// SPDX-License-Identifier: MIT
#include "TrendWidget.h"

#include <QPainter>

#include <algorithm>

namespace qttutorial::avionics {

TrendWidget::TrendWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(300, 160);
}

void TrendWidget::setSeries(std::vector<double> values, double nominalLow, double nominalHigh)
{
    m_values = std::move(values);
    m_nominalLow = nominalLow;
    m_nominalHigh = nominalHigh;
    update();
}

void TrendWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0x1c, 0x1c, 0x1c));

    if (m_values.empty()) {
        painter.setPen(Qt::gray);
        painter.drawText(rect(), Qt::AlignCenter, tr("No data"));
        return;
    }

    const double minValue = std::min(*std::min_element(m_values.begin(), m_values.end()), m_nominalLow) * 0.9;
    const double maxValue = std::max(*std::max_element(m_values.begin(), m_values.end()), m_nominalHigh) * 1.1;
    const double range = std::max(maxValue - minValue, 1e-6);

    const auto toY = [&](double value) {
        return height() - (value - minValue) / range * height();
    };

    const double bandTop = toY(m_nominalHigh);
    const double bandBottom = toY(m_nominalLow);
    painter.fillRect(QRectF(0.0, bandTop, width(), bandBottom - bandTop), QColor(0x2a, 0x4a, 0x2a));

    painter.setPen(QPen(QColor(0x40, 0xd0, 0x40), 2.0));
    QPolygonF polyline;
    const double stepX = m_values.size() > 1 ? width() / static_cast<double>(m_values.size() - 1) : 0.0;
    for (std::size_t i = 0; i < m_values.size(); ++i) {
        polyline << QPointF(static_cast<double>(i) * stepX, toY(m_values[i]));
    }
    painter.drawPolyline(polyline);
}

} // namespace qttutorial::avionics
