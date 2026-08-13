// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::space {

struct GroundTrackState {
    double latitudeDeg = 0.0;  // -90 (south) .. +90 (north)
    double longitudeDeg = 0.0; // -180 .. +180, wraps
};

// Derives an approximate sub-satellite ground-track position from the orbit's
// true anomaly, pure C++23. The satellite's orbital plane is tilted by a fixed
// inclination relative to the equator, and the Earth rotates underneath it at
// its own (much slower) rate, so successive orbits trace westward-drifting
// ground tracks rather than retracing the same line - the same qualitative
// behaviour a real LEO ground-track plot shows. This is a deliberately
// simplified spherical model (no J2 perturbation, no eccentricity), adequate
// for an illustrative telemetry readout, not for mission planning.
class GroundTrackSimulator {
public:
    explicit GroundTrackSimulator(double inclinationDeg = 53.0);

    void step(double dtSeconds, double trueAnomalyDeg);

    [[nodiscard]] const GroundTrackState& state() const { return m_state; }

private:
    double m_inclinationDeg;
    double m_earthRotationDeg = 0.0; // accumulated Earth rotation under the orbit
    GroundTrackState m_state;
};

// True if the great-circle angular distance between the ground track and a
// fixed ground-station latitude/longitude is within `maxAngularRadiusDeg` -
// a simplified stand-in for a station's minimum-elevation visibility circle.
// Pure function, independently testable.
[[nodiscard]] bool isWithinStationView(const GroundTrackState& track, double stationLatitudeDeg,
                                        double stationLongitudeDeg, double maxAngularRadiusDeg);

} // namespace qttutorial::space
