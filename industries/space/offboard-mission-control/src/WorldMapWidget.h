// SPDX-License-Identifier: MIT
#pragma once

#include "GroundStationTracker.h"
#include "Satellite.h"

#include <QHash>
#include <QString>
#include <QWidget>

#include <deque>
#include <vector>

QT_BEGIN_NAMESPACE
class QPainter;
QT_END_NAMESPACE

namespace qttutorial::space {

// A flattened equirectangular ground-track map (QPainter only - no real
// mapping/GIS library, no network map tiles), in the visual language of a
// mission-control world-map display: a latitude/longitude graticule with the
// equator emphasized, a handful of stylised (not geographically precise)
// landmass silhouettes for orientation, each fleet satellite's current
// position plus a short fading trail of recent positions, and the fixed
// ground-station markers from GroundStationTracker.
class WorldMapWidget : public QWidget {
    Q_OBJECT
public:
    explicit WorldMapWidget(std::vector<GroundStation> stations, QWidget* parent = nullptr);

    void setFleet(const std::vector<SatelliteState>& satellites);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    [[nodiscard]] QPointF toScreen(double latitudeDeg, double longitudeDeg) const;
    void paintGraticule(QPainter& painter) const;
    void paintLandmasses(QPainter& painter) const;
    void paintGroundStations(QPainter& painter) const;
    void paintFleet(QPainter& painter) const;

    std::vector<GroundStation> m_stations;
    std::vector<SatelliteState> m_satellites;
    QHash<QString, std::deque<QPointF>> m_trails;
};

} // namespace qttutorial::space
