// SPDX-License-Identifier: MIT
#include "AlarmEvaluator.h"

namespace qttutorial::plant_scada {

Severity evaluate(const TagDefinition& tag, double value)
{
    if (value <= tag.criticalLow || value >= tag.criticalHigh) {
        return Severity::Critical;
    }
    if (value <= tag.warnLow || value >= tag.warnHigh) {
        return Severity::Warning;
    }
    return Severity::Normal;
}

const char* severityLabel(Severity severity)
{
    switch (severity) {
    case Severity::Normal:
        return "Normal";
    case Severity::Warning:
        return "Warning";
    case Severity::Critical:
        return "Critical";
    }
    return "Unknown";
}

} // namespace qttutorial::plant_scada
