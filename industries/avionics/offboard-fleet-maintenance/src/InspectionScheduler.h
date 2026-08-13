// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::avionics {

enum class InspectionUrgency { Nominal, DueSoon, Urgent };

struct InspectionStatus {
    double hoursRemaining = 0.0;
    int cyclesRemaining = 0;
    InspectionUrgency urgency = InspectionUrgency::Nominal;
};

// A pure C++23, Qt-free heuristic for when a scheduled inspection is due, and
// how urgently. It is deliberately simple (a fixed hours/cycles interval, like
// a real "A-check" style utilization limit) but it genuinely *correlates* two
// independent signals rather than treating them separately: an aircraft that
// is merely close to its next scheduled inspection is only "due soon", but one
// that is close *and* already has an active engine-parameter anomaly is
// escalated to "urgent" - the same way a real operations desk would prioritize
// an aircraft that is both due for a look and already showing a symptom.
class InspectionScheduler {
public:
    static constexpr double kIntervalHours = 400.0;
    static constexpr int kIntervalCycles = 600;
    static constexpr double kDueSoonHoursThreshold = 40.0;
    static constexpr int kDueSoonCyclesThreshold = 60;

    [[nodiscard]] InspectionStatus evaluate(double flightHoursSinceInspection, int cyclesSinceInspection,
                                             bool hasActiveAnomaly) const;
};

} // namespace qttutorial::avionics
