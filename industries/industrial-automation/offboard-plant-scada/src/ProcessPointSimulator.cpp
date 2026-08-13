// SPDX-License-Identifier: MIT
#include "ProcessPointSimulator.h"

#include <QHash>

#include <algorithm>
#include <cmath>
#include <numbers>

namespace qttutorial::plant_scada {

namespace {
// Every line runs for kRunSeconds then stops for kStopSeconds, deterministic
// and staggered per line by a phase offset derived from the line name's
// hash, so lines don't all start/stop in lockstep the way three separate
// physical cells realistically wouldn't.
constexpr double kRunSeconds = 200.0;
constexpr double kStopSeconds = 40.0;
constexpr double kCycleSeconds = kRunSeconds + kStopSeconds;
constexpr double kResidualFraction = 0.02; // small non-zero residual, not a hard zero
}

const std::vector<TagDefinition>& defaultTags()
{
    static const std::vector<TagDefinition> tags{
        {"TK101.LEVEL", "%", "Line 1 - Filling", 55.0, 12.0, 240.0, 0.0, 15.0, 85.0, 5.0, 95.0, false},
        {"FT201.FLOW", "m3/min", "Line 1 - Filling", 4.5, 1.5, 90.0, 0.02, 1.0, 8.0, 0.0, 9.0, true},
        {"PT301.PRESSURE", "bar", "Line 2 - Reaction", 5.0, 0.8, 45.0, 0.0, 2.0, 7.0, 1.0, 7.5, false},
        {"MZ301.MOTOR_SPEED", "%", "Line 2 - Reaction", 82.0, 4.0, 30.0, 0.0, 20.0, 95.0, 5.0, 100.0, true},
        {"TT401.TEMP", "degC", "Line 3 - Packaging", 68.0, 3.0, 300.0, 0.05, 55.0, 82.0, 45.0, 90.0, false},
        {"CV401.CONV_SPEED", "m/min", "Line 3 - Packaging", 1.1, 0.15, 20.0, 0.0, 0.3, 1.8, 0.0, 2.0, true},
    };
    return tags;
}

std::vector<QString> lineNames()
{
    std::vector<QString> names;
    for (const TagDefinition& tag : defaultTags()) {
        if (std::find(names.begin(), names.end(), tag.line) == names.end()) {
            names.push_back(tag.line);
        }
    }
    return names;
}

bool lineRunningAt(const QString& line, double tSeconds)
{
    const double phaseOffset = static_cast<double>(qHash(line) % 1000) / 1000.0 * kCycleSeconds;
    const double phase = std::fmod(tSeconds + phaseOffset, kCycleSeconds);
    return phase < kRunSeconds;
}

double valueAt(const TagDefinition& tag, double tSeconds)
{
    const double angular = 2.0 * std::numbers::pi / tag.periodSeconds;
    const double periodic = tag.amplitude * std::sin(angular * tSeconds);
    const double drift = tag.driftPerHour * (tSeconds / 3600.0);
    const double nominal = tag.baseValue + periodic + drift;

    if (tag.gatedByLineRunning && !lineRunningAt(tag.line, tSeconds)) {
        // Not a hard zero: a stopped conveyor/pump still reads a small
        // residual (creep, leakage, sensor noise floor) rather than an
        // implausibly exact 0.000.
        return tag.baseValue * kResidualFraction;
    }
    return nominal;
}

} // namespace qttutorial::plant_scada
