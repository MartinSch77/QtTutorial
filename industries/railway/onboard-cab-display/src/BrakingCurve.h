// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::cab_display {

// An ETCS-style speed supervision curve: the highest speed from which the
// train can still decelerate at `decelerationMs2` and reach `targetSpeedKmh`
// exactly at the restriction, `distanceToRestrictionM` ahead. Pure function
// of its inputs, which is what makes the driving-advice bar's "margin to
// next restriction" testable without simulating a train at all.
[[nodiscard]] double maxSafeSpeedKmh(double distanceToRestrictionM, double targetSpeedKmh, double decelerationMs2);

// A hysteresis-based brake-warning latch, mirroring the way a real ETCS/ATP
// system debounces its brake-intervention indication instead of flickering
// exactly at the supervision limit: once active, `speedKmh` must fall back
// to `permittedSpeedKmh + offMarginKmh` or below before it clears, but it
// only becomes active once `speedKmh` exceeds `permittedSpeedKmh +
// onMarginKmh`. Pure function of its inputs (including the previous state),
// so it is testable without simulating a train.
[[nodiscard]] bool nextBrakeWarningState(bool currentlyActive, double speedKmh, double permittedSpeedKmh,
                                          double onMarginKmh, double offMarginKmh);

} // namespace qttutorial::cab_display
