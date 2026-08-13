// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::fleet_ops {

struct MapPoint {
    double x = 0.0;
    double y = 0.0;
};

// Maps a train's position on the shared loop route to a point on a circle of
// the given radius, centred at the origin, so the schematic fleet map's
// train markers and station ticks are geometrically derived from the same
// `positionKm`/`loopLengthKm` values the table already shows rather than
// picked independently. Position 0 is placed at the top (12 o'clock) and
// increasing position sweeps clockwise, matching the direction of travel.
[[nodiscard]] MapPoint positionOnLoop(double positionKm, double loopLengthKm, double radius);

} // namespace qttutorial::fleet_ops
