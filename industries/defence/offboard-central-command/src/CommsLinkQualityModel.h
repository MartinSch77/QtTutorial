// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::defence {

// Pure, stateless helper: how many simulation ticks pass between updates to
// an asset's *displayed* position/telemetry, given a 0-100 comms-link
// quality percentage for that asset. This is a data-quality effect only -
// it decides how fresh a track looks on the tactical map, never anything
// about the tracked platform's engagement status.
[[nodiscard]] int commsLinkUpdateIntervalTicks(double linkQualityPercent);

} // namespace qttutorial::defence
