// SPDX-License-Identifier: MIT
#include "TrendWidget.h"

#include <QPainter>
#include <QPainterPath>

#include <algorithm>

namespace qttutorial::plant_scada {

TrendWidget::TrendWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(220);
}

void TrendWidget::setSamples(const std::vector<Sample>& samples, double lowLimit, double highLimit)
{
    m_samples = samples;
    m_lowLimit = lowLimit;
    m_highLimit = highLimit;
    update();
}

void TrendWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#101820"));

    const QRectF plotArea = rect().adjusted(48, 12, -12, -24);
    painter.setPen(QColor("#3a4452"));
    painter.drawRect(plotArea);

    if (m_samples.size() < 2 || m_highLimit <= m_lowLimit) {
        painter.setPen(QColor("#8a94a3"));
        painter.drawText(rect(), Qt::AlignCenter, tr("No trend data for this range"));
        return;
    }

    const qint64 tFirst = m_samples.front().timestamp.toMSecsSinceEpoch();
    const qint64 tLast = m_samples.back().timestamp.toMSecsSinceEpoch();
    const qint64 span = std::max<qint64>(tLast - tFirst, 1);

    auto toPoint = [&](const Sample& sample) {
        const double xFrac = static_cast<double>(sample.timestamp.toMSecsSinceEpoch() - tFirst)
            / static_cast<double>(span);
        const double yFrac = (sample.value - m_lowLimit) / (m_highLimit - m_lowLimit);
        const double x = plotArea.left() + xFrac * plotArea.width();
        const double y = plotArea.bottom() - std::clamp(yFrac, 0.0, 1.0) * plotArea.height();
        return QPointF(x, y);
    };

    QPainterPath path;
    path.moveTo(toPoint(m_samples.front()));
    for (const Sample& sample : m_samples) {
        path.lineTo(toPoint(sample));
    }
    painter.setPen(QPen(QColor("#2f81f7"), 2.0));
    painter.drawPath(path);

    painter.setPen(QColor("#8a94a3"));
    painter.drawText(QRectF(0, plotArea.top() - 6, 44, 16), Qt::AlignRight, QString::number(m_highLimit, 'f', 1));
    painter.drawText(QRectF(0, plotArea.bottom() - 10, 44, 16), Qt::AlignRight, QString::number(m_lowLimit, 'f', 1));
}

} // namespace qttutorial::plant_scada
