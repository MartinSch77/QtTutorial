// SPDX-License-Identifier: MIT
#include "Satellite.h"

namespace qttutorial::space {

HealthSummary classifyHealth(double batteryPercent, bool inEclipse)
{
    if (batteryPercent < 15.0) {
        return HealthSummary::Critical;
    }
    if (batteryPercent < 30.0 || (batteryPercent < 40.0 && inEclipse)) {
        return HealthSummary::Caution;
    }
    return HealthSummary::Nominal;
}

QString toString(HealthSummary health)
{
    switch (health) {
    case HealthSummary::Nominal:
        return QStringLiteral("Nominal");
    case HealthSummary::Caution:
        return QStringLiteral("Caution");
    case HealthSummary::Critical:
        return QStringLiteral("Critical");
    }
    return QStringLiteral("Unknown");
}

} // namespace qttutorial::space
