// SPDX-License-Identifier: MIT
#include "InspectionScheduler.h"

#include <algorithm>

namespace qttutorial::avionics {

InspectionStatus InspectionScheduler::evaluate(double flightHoursSinceInspection, int cyclesSinceInspection,
                                                 bool hasActiveAnomaly) const
{
    InspectionStatus status;
    status.hoursRemaining = std::max(0.0, kIntervalHours - flightHoursSinceInspection);
    status.cyclesRemaining = std::max(0, kIntervalCycles - cyclesSinceInspection);

    const bool dueSoon =
        status.hoursRemaining <= kDueSoonHoursThreshold || status.cyclesRemaining <= kDueSoonCyclesThreshold;
    const bool overdue = status.hoursRemaining <= 0.0 || status.cyclesRemaining <= 0;

    if (overdue || (dueSoon && hasActiveAnomaly)) {
        status.urgency = InspectionUrgency::Urgent;
    } else if (dueSoon) {
        status.urgency = InspectionUrgency::DueSoon;
    } else {
        status.urgency = InspectionUrgency::Nominal;
    }

    return status;
}

} // namespace qttutorial::avionics
