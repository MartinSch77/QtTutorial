// SPDX-License-Identifier: MIT
#pragma once

#include "Asset.h"

#include <vector>

namespace qttutorial::defence {

// A simple green/amber/red roll-up of the fleet's health classifications,
// for the fleet-readiness summary board. Pure counting logic, no Qt
// dependency, so it is trivially unit-testable.
struct FleetReadinessSummary {
    int nominal = 0;
    int caution = 0;
    int critical = 0;

    [[nodiscard]] int total() const { return nominal + caution + critical; }
};

[[nodiscard]] FleetReadinessSummary summarizeReadiness(const std::vector<Asset>& assets);

} // namespace qttutorial::defence
