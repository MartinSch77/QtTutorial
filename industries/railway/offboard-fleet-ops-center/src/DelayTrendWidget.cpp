// SPDX-License-Identifier: MIT
#include "DelayTrendWidget.h"

#include <QPainter>
#include <QPainterPath>

#include <algorithm>

namespace qttutorial::fleet_ops {

DelayTrendWidget::DelayTrendWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(200);
}

void DelayTrendWidget::setSamples(const std::vector<RunSample>& samples)
{
    m_samples = samples;
    update();
}

void DelayTrendWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#101820"));

    const QRectF plotArea = rect().adjusted(48, 12, -12, -20);
    painter.setPen(QColor("#3a4452"));
    painter.drawRect(plotArea);

    if (m_samples.size() < 2) {
        painter.setPen(QColor("#8a94a3"));
        painter.drawText(rect(), Qt::AlignCenter, tr("No delay history for this train yet"));
        return;
    }

    double maxAbsDelay = 1.0;
    for (const RunSample& sample : m_samples) {
        maxAbsDelay = std::max(maxAbsDelay, std::abs(sample.delayMinutes));
    }
    maxAbsDelay *= 1.15;

    const qint64 tFirst = m_samples.front().timestamp.toMSecsSinceEpoch();
    const qint64 tLast = m_samples.back().timestamp.toMSecsSinceEpoch();
    const qint64 span = std::max<qint64>(tLast - tFirst, 1);

    auto toPoint = [&](const RunSample& sample) {
        const double xFrac
            = static_cast<double>(sample.timestamp.toMSecsSinceEpoch() - tFirst) / static_cast<double>(span);
        const double yFrac = (sample.delayMinutes + maxAbsDelay) / (2.0 * maxAbsDelay);
        const double x = plotArea.left() + xFrac * plotArea.width();
        const double y = plotArea.bottom() - std::clamp(yFrac, 0.0, 1.0) * plotArea.height();
        return QPointF(x, y);
    };

    const double zeroY = plotArea.bottom() - (maxAbsDelay / (2.0 * maxAbsDelay)) * plotArea.height();
    painter.setPen(QPen(QColor("#3a4452"), 1.0, Qt::DashLine));
    painter.drawLine(QPointF(plotArea.left(), zeroY), QPointF(plotArea.right(), zeroY));

    QPainterPath path;
    path.moveTo(toPoint(m_samples.front()));
    for (const RunSample& sample : m_samples) {
        path.lineTo(toPoint(sample));
    }
    painter.setPen(QPen(QColor("#3ddc84"), 2.0));
    painter.drawPath(path);

    painter.setPen(QColor("#8a94a3"));
    painter.drawText(QRectF(0, plotArea.top() - 6, 44, 16), Qt::AlignRight,
                      QStringLiteral("+%1").arg(maxAbsDelay, 0, 'f', 0));
    painter.drawText(QRectF(0, plotArea.bottom() - 10, 44, 16), Qt::AlignRight,
                      QStringLiteral("-%1").arg(maxAbsDelay, 0, 'f', 0));
}

} // namespace qttutorial::fleet_ops
