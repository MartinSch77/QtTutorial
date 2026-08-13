// SPDX-License-Identifier: MIT
#include "ProcessPointSimulator.h"

#include <cmath>
#include <numbers>

namespace qttutorial::plant_scada {

const std::vector<TagDefinition>& defaultTags()
{
    static const std::vector<TagDefinition> tags{
        {"TK101.LEVEL", "%", 55.0, 12.0, 240.0, 0.0, 15.0, 85.0, 5.0, 95.0},
        {"FT201.FLOW", "m3/min", 4.5, 1.5, 90.0, 0.02, 1.0, 8.0, 0.0, 9.0},
        {"PT301.PRESSURE", "bar", 5.0, 0.8, 45.0, 0.0, 2.0, 7.0, 1.0, 7.5},
        {"TT401.TEMP", "degC", 68.0, 3.0, 300.0, 0.05, 55.0, 82.0, 45.0, 90.0},
    };
    return tags;
}

double valueAt(const TagDefinition& tag, double tSeconds)
{
    const double angular = 2.0 * std::numbers::pi / tag.periodSeconds;
    const double periodic = tag.amplitude * std::sin(angular * tSeconds);
    const double drift = tag.driftPerHour * (tSeconds / 3600.0);
    return tag.baseValue + periodic + drift;
}

} // namespace qttutorial::plant_scada
