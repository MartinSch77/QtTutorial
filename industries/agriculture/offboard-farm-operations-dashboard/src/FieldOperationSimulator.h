// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

namespace qttutorial::agriculture::ops {

struct FieldSample {
    QString fieldId;
    QString vehicleId;
    int passNumber = 1;
    double coveragePercent = 0.0;
    QString status;               // per-tick status: "scheduled"/"idle"/"turning"/"working"/"complete"
    QString overallStatus;        // farm-wide job status: "not_started"/"in_progress"/"complete"
    int plannedPasses = 1;        // total passes needed to finish this field (varies with field size)
    double overallFieldProgressPercent = 0.0; // progress across the whole job, not just the current pass
    double engineLoadPercent = 0.0;
    double fuelLevelPercent = 100.0;
};

// Stands in for a farm-management telemetry-ingestion service: given a field
// index and an elapsed time, produces a deterministic, physically plausible
// sample. Fields are dispatched at staggered times (as a real crew would send
// machines to different fields in sequence) rather than all starting at once,
// each field has its own planned pass count standing in for field size, and
// every field still follows the same believable pattern once dispatched: a
// short idle/downtime window at the start of each pass, coverage advancing
// steadily while working, short turning windows at each row end, and engine
// load / fuel-burn rate correlated with that same working/turning/idle state
// (higher load and faster burn while working) so the readouts stay
// physically consistent with one another rather than varying independently.
class FieldOperationSimulator {
public:
    static constexpr double kPassPeriodSeconds = 240.0;
    static constexpr double kIdleFraction = 0.08;
    static constexpr double kTurnZoneFraction = 0.04;
    static constexpr double kDispatchDelaySecondsPerField = 40.0;
    static constexpr double kEngineLoadIdlePercent = 18.0;
    static constexpr double kEngineLoadTurningPercent = 45.0;
    static constexpr double kEngineLoadWorkingPercent = 78.0;
    static constexpr double kFuelBurnBaselinePercentPerSecond = 0.0016;
    static constexpr double kFuelBurnPercentPerSecondPerLoadPoint = 0.00024;
    static constexpr double kFuelFloorPercent = 8.0;

    [[nodiscard]] static FieldSample sampleAt(int fieldIndex, double elapsedSeconds);

    [[nodiscard]] static double dispatchDelaySecondsFor(int fieldIndex);
    [[nodiscard]] static int plannedPassesFor(int fieldIndex);
    [[nodiscard]] static double engineLoadForStatus(const QString& status);
    [[nodiscard]] static double fuelBurnPercentPerSecondAt(double engineLoadPercent);

private:
    [[nodiscard]] static QString fieldIdFor(int fieldIndex);
    [[nodiscard]] static QString vehicleIdFor(int fieldIndex);
    [[nodiscard]] static double averageBurnPercentPerSecond();
};

} // namespace qttutorial::agriculture::ops
