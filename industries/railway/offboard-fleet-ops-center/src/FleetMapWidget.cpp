// SPDX-License-Identifier: MIT
#include "FleetMapWidget.h"
#include "DelayCalculator.h"
#include "FleetMapGeometry.h"

#include <QPainter>
#include <QPainterPath>

#include <cmath>
#include <numbers>

namespace qttutorial::fleet_ops {

namespace {

QColor punctualityColor(Punctuality punctuality)
{
    switch (punctuality) {
    case Punctuality::Early:
        return QColor("#2f81f7");
    case Punctuality::OnTime:
        return QColor("#3ddc84");
    case Punctuality::MinorDelay:
        return QColor("#e5b93d");
    case Punctuality::MajorDelay:
        return QColor("#e5484d");
    }
    return QColor("#8a94a3");
}

// A small train-front silhouette (the same visual language as the onboard
// cab display's icon set, redrawn here with QPainterPath since this app is
// Qt Widgets rather than QML), used for each marker instead of a plain dot.
void paintTrainIcon(QPainter& painter, const QPointF& centre, double headingRad, const QColor& color)
{
    painter.save();
    painter.translate(centre);
    painter.rotate(headingRad * 180.0 / std::numbers::pi + 90.0);

    QPainterPath path;
    path.moveTo(-6, 6);
    path.lineTo(-6, -1);
    path.cubicTo(-6, -7, -3, -9, 0, -9);
    path.cubicTo(3, -9, 6, -7, 6, -1);
    path.lineTo(6, 6);
    path.closeSubpath();

    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawPath(path);
    painter.restore();
}

} // namespace

FleetMapWidget::FleetMapWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(220);
}

void FleetMapWidget::setFleet(const FleetConfig& config, const std::vector<TrainState>& states)
{
    m_config = config;
    m_states = states;
    update();
}

void FleetMapWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor("#101820"));

    const QPointF centre = rect().center();
    const double radius = 0.42 * std::min(width(), height());
    if (radius <= 0.0 || m_config.loopLengthKm <= 0.0) {
        painter.setPen(QColor("#8a94a3"));
        painter.drawText(rect(), Qt::AlignCenter, tr("No fleet data yet"));
        return;
    }

    // The loop line itself.
    painter.setPen(QPen(QColor("#3a4452"), 3.0));
    painter.drawEllipse(centre, radius, radius);

    // Station ticks, evenly spaced by the same station spacing the table's
    // "next stop" column uses.
    const int stationCount
        = std::max(1, static_cast<int>(std::round(m_config.loopLengthKm / m_config.stationSpacingKm)));
    painter.setPen(QPen(QColor("#5a6576"), 2.0));
    for (int i = 0; i < stationCount; ++i) {
        const double positionKm = i * m_config.stationSpacingKm;
        const MapPoint inner = positionOnLoop(positionKm, m_config.loopLengthKm, radius - 6.0);
        const MapPoint outer = positionOnLoop(positionKm, m_config.loopLengthKm, radius + 6.0);
        painter.drawLine(centre + QPointF(inner.x, inner.y), centre + QPointF(outer.x, outer.y));
    }

    // One marker per train, coloured by live punctuality and oriented along
    // its direction of travel around the loop.
    for (const TrainState& state : m_states) {
        const MapPoint point = positionOnLoop(state.positionKm, m_config.loopLengthKm, radius);
        const MapPoint ahead
            = positionOnLoop(state.positionKm + 0.15, m_config.loopLengthKm, radius);
        const double heading = std::atan2(ahead.y - point.y, ahead.x - point.x);
        const QColor color = punctualityColor(classifyDelay(state.delayMinutes));

        paintTrainIcon(painter, centre + QPointF(point.x, point.y), heading, color);

        painter.setPen(QColor("#8a94a3"));
        painter.drawText(centre + QPointF(point.x + 10, point.y + 4), state.trainId);
    }
}

} // namespace qttutorial::fleet_ops
