// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::cab_display {

// An ETCS-style speed supervision curve: the highest speed from which the
// train can still decelerate at `decelerationMs2` and reach `targetSpeedKmh`
// exactly at the restriction, `distanceToRestrictionM` ahead. Pure function
// of its inputs, which is what makes the driving-advice bar's "margin to
// next restriction" testable without simulating a train at all.
[[nodiscard]] double maxSafeSpeedKmh(double distanceToRestrictionM, double targetSpeedKmh, double decelerationMs2);

} // namespace qttutorial::cab_display
