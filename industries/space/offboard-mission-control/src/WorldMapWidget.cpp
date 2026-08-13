// SPDX-License-Identifier: MIT
#include "WorldMapWidget.h"

#include "IconPainter.h"

#include <QPainter>
#include <QPainterPath>

#include <cmath>
#include <utility>

namespace qttutorial::space {

namespace {
constexpr int kMaxTrailPoints = 40;

// A handful of very roughly-shaped, stylised landmass silhouettes, purely
// for visual orientation on the graticule - not geographically precise
// outlines and not sourced from any mapping dataset or copyrighted asset.
// Coordinates are (longitudeDeg, latitudeDeg) corner points of simple
// polygons.
const std::vector<std::vector<QPointF>>& stylisedLandmasses()
{
    static const std::vector<std::vector<QPointF>> landmasses = {
        // Rough Africa/Europe-ish blob.
        {{-10, 35}, {30, 38}, {35, 15}, {20, -10}, {-5, -20}, {-15, 5}, {-10, 35}},
        // Rough Asia-ish blob.
        {{40, 45}, {100, 55}, {140, 40}, {120, 15}, {75, 10}, {40, 25}, {40, 45}},
        // Rough Americas-ish blob.
        {{-120, 45}, {-95, 50}, {-75, 15}, {-70, -20}, {-90, -35}, {-105, 0}, {-120, 45}},
        // Rough Australia-ish blob.
        {{115, -15}, {145, -12}, {150, -30}, {125, -35}, {115, -15}},
    };
    return landmasses;
}
}

WorldMapWidget::WorldMapWidget(std::vector<GroundStation> stations, QWidget* parent)
    : QWidget(parent)
    , m_stations(std::move(stations))
{
    setMinimumSize(480, 260);
}

void WorldMapWidget::setFleet(const std::vector<SatelliteState>& satellites)
{
    m_satellites = satellites;
    for (const SatelliteState& satellite : m_satellites) {
        std::deque<QPointF>& trail = m_trails[satellite.name];
        trail.push_back(QPointF(satellite.longitudeDeg, satellite.latitudeDeg));
        while (trail.size() > kMaxTrailPoints) {
            trail.pop_front();
        }
    }
    update();
}

QPointF WorldMapWidget::toScreen(double latitudeDeg, double longitudeDeg) const
{
    const double x = (longitudeDeg + 180.0) / 360.0 * width();
    const double y = (90.0 - latitudeDeg) / 180.0 * height();
    return {x, y};
}

void WorldMapWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(0x08, 0x0c, 0x14));

    paintGraticule(painter);
    paintLandmasses(painter);
    paintGroundStations(painter);
    paintFleet(painter);
}

void WorldMapWidget::paintGraticule(QPainter& painter) const
{
    // A meridian/parallel every 30 degrees, equator and prime meridian
    // emphasised.
    for (int lon = -180; lon <= 180; lon += 30) {
        const double x = toScreen(0.0, lon).x();
        painter.setPen(QPen(lon == 0 ? QColor(0x50, 0x60, 0x78) : QColor(0x20, 0x28, 0x38), lon == 0 ? 1.2 : 0.6));
        painter.drawLine(QPointF(x, 0.0), QPointF(x, height()));
    }
    for (int lat = -90; lat <= 90; lat += 30) {
        const double y = toScreen(lat, 0.0).y();
        painter.setPen(QPen(lat == 0 ? QColor(0x50, 0x60, 0x78) : QColor(0x20, 0x28, 0x38), lat == 0 ? 1.2 : 0.6));
        painter.drawLine(QPointF(0.0, y), QPointF(width(), y));
    }
}

void WorldMapWidget::paintLandmasses(QPainter& painter) const
{
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0x18, 0x28, 0x22));
    for (const auto& polygon : stylisedLandmasses()) {
        QPainterPath path;
        for (std::size_t i = 0; i < polygon.size(); ++i) {
            const QPointF screenPoint = toScreen(polygon[i].y(), polygon[i].x());
            if (i == 0) {
                path.moveTo(screenPoint);
            } else {
                path.lineTo(screenPoint);
            }
        }
        path.closeSubpath();
        painter.drawPath(path);
    }
}

void WorldMapWidget::paintGroundStations(QPainter& painter) const
{
    for (const GroundStation& station : m_stations) {
        const QPointF p = toScreen(station.latitudeDeg, station.longitudeDeg);
        icons::drawGroundStation(painter, QRectF(p.x() - 10.0, p.y() - 16.0, 20.0, 20.0), QColor(0xd0, 0xa0, 0x30),
                                  false);
    }
}

void WorldMapWidget::paintFleet(QPainter& painter) const
{
    // Fading trail plus a satellite icon at the current position,
    // colour-coded by health.
    for (const SatelliteState& satellite : m_satellites) {
        const QColor healthColor = satellite.health == HealthSummary::Critical ? QColor(0xe0, 0x40, 0x30)
                                     : satellite.health == HealthSummary::Caution ? QColor(0xd0, 0xa0, 0x30)
                                                                                   : QColor(0x40, 0xc0, 0xff);

        const auto trailIt = m_trails.constFind(satellite.name);
        if (trailIt != m_trails.constEnd()) {
            const std::deque<QPointF>& trail = trailIt.value();
            painter.setPen(QPen(healthColor, 1.5));
            for (std::size_t i = 1; i < trail.size(); ++i) {
                // Skip the segment that wraps across the +/-180 degree seam
                // rather than drawing a spurious line across the whole map.
                if (std::abs(trail[i].x() - trail[i - 1].x()) > 180.0) {
                    continue;
                }
                painter.drawLine(toScreen(trail[i - 1].y(), trail[i - 1].x()), toScreen(trail[i].y(), trail[i].x()));
            }
        }

        const QPointF p = toScreen(satellite.latitudeDeg, satellite.longitudeDeg);
        icons::drawSatellite(painter, QRectF(p.x() - 12.0, p.y() - 12.0, 24.0, 24.0), healthColor);
        painter.setPen(Qt::white);
        painter.drawText(p + QPointF(14.0, 4.0), satellite.name);
    }
}

} // namespace qttutorial::space
