// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::fleet_ops {

enum class Punctuality { OnTime, MinorDelay, MajorDelay, Early };

// Pure classification of a delay figure (minutes late; negative means
// early) against fixed thresholds, mirroring how a real operations centre
// buckets punctuality for reporting.
[[nodiscard]] Punctuality classifyDelay(double delayMinutes);

[[nodiscard]] const char* punctualityLabel(Punctuality punctuality);

} // namespace qttutorial::fleet_ops
