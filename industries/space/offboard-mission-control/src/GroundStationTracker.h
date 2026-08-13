// SPDX-License-Identifier: MIT
#pragma once

#include "Satellite.h"

#include <QString>

#include <optional>
#include <vector>

namespace qttutorial::space {

struct GroundStation {
    QString name;
    double latitudeDeg = 0.0;
    double longitudeDeg = 0.0;
    double visibilityRadiusDeg = 0.0; // simplified stand-in for a minimum-elevation contact circle
};

struct ContactWindow {
    QString satelliteName;
    QString stationName;
    bool inContactNow = false;
    // Only meaningful when !inContactNow: an estimate of when the satellite's
    // ground track will next fall inside the station's visibility circle,
    // found by forward-sampling the fleet's own phase-advance model.
    double minutesUntilNextContact = 0.0;
};

// A small, fixed constellation of fictitious ground stations plus a
// deliberately simple "next contact" predictor: it re-uses the same circular
// -orbit ground-track model as FleetOrbitSimulator/GroundTrackMath and just
// samples forward in time until a satellite's predicted ground track falls
// within a station's visibility circle. Pure C++23, no Qt Widgets
// dependency, independently unit-testable.
class GroundStationTracker {
public:
    GroundStationTracker();

    [[nodiscard]] const std::vector<GroundStation>& stations() const { return m_stations; }

    // Finds the nearest (in time) contact window for one satellite across all
    // configured stations, sampling forward by `stepMinutes` up to
    // `maxLookaheadMinutes`. Returns std::nullopt only if the satellite is not
    // currently in contact and no contact is found within the lookahead
    // window (should not happen with these station/orbit parameters, but the
    // model makes no promises beyond the lookahead horizon).
    [[nodiscard]] std::optional<ContactWindow> nextContact(const SatelliteState& satellite,
                                                            double orbitalPeriodMinutes, double stepMinutes = 0.5,
                                                            double maxLookaheadMinutes = 200.0) const;

private:
    std::vector<GroundStation> m_stations;
};

} // namespace qttutorial::space
