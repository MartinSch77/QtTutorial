// SPDX-License-Identifier: MIT
#include "FleetReadiness.h"

namespace qttutorial::defence {

FleetReadinessSummary summarizeReadiness(const std::vector<Asset>& assets)
{
    FleetReadinessSummary summary;
    for (const Asset& asset : assets) {
        if (asset.health == QStringLiteral("Critical")) {
            ++summary.critical;
        } else if (asset.health == QStringLiteral("Caution")) {
            ++summary.caution;
        } else {
            ++summary.nominal;
        }
    }
    return summary;
}

} // namespace qttutorial::defence
