// SPDX-License-Identifier: MIT
#pragma once

#include "AlertLevel.h"

#include <QString>

#include <vector>

namespace qttutorial::startrek_bridge {

// A simulated nearby object shown on the helm's local-space plot: another
// vessel, a planetoid, an asteroid or a debris field. Purely flavour data
// for a "what's out there" plot, not a targeting or collision system.
struct NavContact {
    int id = 0;
    QString label;
    double xKm = 0.0;
    double yKm = 0.0;
    double headingDeg = 0.0;
    double speedKmPerMin = 0.0;
};

// Drives the Helm/Navigation station: ship heading, a fictional "warp
// factor" (0-9.9) and sub-light "impulse" percentage, plus a handful of
// NavContacts drifting smoothly within a bounded local-space area. Pure
// C++23, deterministic given elapsed time and the current AlertLevel, so it
// is unit-testable without Qt Quick.
class NavigationSimulator {
public:
    explicit NavigationSimulator(double areaRadiusKm = 50.0);

    void advance(double dtSeconds, AlertLevel alertLevel);

    [[nodiscard]] double headingDeg() const { return m_headingDeg; }
    [[nodiscard]] double warpFactor() const { return m_warpFactor; }
    [[nodiscard]] double impulsePercent() const { return m_impulsePercent; }
    [[nodiscard]] const std::vector<NavContact>& contacts() const { return m_contacts; }
    [[nodiscard]] double areaRadiusKm() const { return m_areaRadiusKm; }

private:
    void seedContacts();

    double m_headingDeg = 30.0;
    double m_warpFactor = 0.0;
    double m_impulsePercent = 0.0;
    std::vector<NavContact> m_contacts;
    std::vector<double> m_turnRateDegPerSec;
    double m_areaRadiusKm;
    double m_elapsedSeconds = 0.0;
};

} // namespace qttutorial::startrek_bridge
