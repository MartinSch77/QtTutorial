// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::space {

struct GroundTrackPosition {
    double latitudeDeg = 0.0;  // -90 (south) .. +90 (north)
    double longitudeDeg = 0.0; // -180 .. +180, wraps
};

// Pure spherical-orbit ground-track math, shared logic between fleet members:
// given an orbital phase (true anomaly from the ascending node), a fixed
// inclination and how far Earth has rotated underneath the orbit so far,
// derive the sub-satellite latitude/longitude. Deliberately simplified (no
// J2 perturbation, no eccentricity) - adequate for an illustrative fleet
// ground-track map, not for mission planning. Pure C++23, independently
// testable without Qt Widgets or a running application.
[[nodiscard]] GroundTrackPosition computeGroundTrack(double phaseDeg, double inclinationDeg,
                                                       double earthRotationDeg);

// Great-circle angular distance in degrees between two lat/lon points, via
// the spherical law of cosines.
[[nodiscard]] double angularDistanceDeg(double lat1Deg, double lon1Deg, double lat2Deg, double lon2Deg);

} // namespace qttutorial::space
