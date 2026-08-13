// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

#include <vector>

namespace qttutorial::defence {

// A navigation waypoint marker, shown as bearing/distance relative to the
// wearer - the kind of thing an AR HUD overlays on the world, not a target.
struct HudWaypoint {
    QString label;
    double bearingDeg = 0.0;
    double distanceKm = 0.0;
};

// A friendly teammate position indicator, shown the same way as waypoints.
struct HudTeammate {
    int id = 0;
    double bearingDeg = 0.0;
    double distanceKm = 0.0;
};

// Drives the AR HUD overlay panel: own-platform compass heading and a small
// set of waypoint/teammate bearing indicators that drift smoothly over time.
// Pure C++23, deterministic given elapsed time.
class HudSimulator {
public:
    HudSimulator();

    void advance(double dtSeconds);

    [[nodiscard]] double headingDeg() const { return m_headingDeg; }
    [[nodiscard]] const std::vector<HudWaypoint>& waypoints() const { return m_waypoints; }
    [[nodiscard]] const std::vector<HudTeammate>& teammates() const { return m_teammates; }

private:
    double m_headingDeg = 0.0;
    std::vector<HudWaypoint> m_waypoints;
    std::vector<HudTeammate> m_teammates;
    double m_elapsedSeconds = 0.0;
};

} // namespace qttutorial::defence
