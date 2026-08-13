// SPDX-License-Identifier: MIT
#include "CommsLinkQualityModel.h"

#include <algorithm>

namespace qttutorial::defence {

int commsLinkUpdateIntervalTicks(double linkQualityPercent)
{
    const double clamped = std::clamp(linkQualityPercent, 0.0, 100.0);
    return 1 + static_cast<int>((100.0 - clamped) / 15.0);
}

} // namespace qttutorial::defence
