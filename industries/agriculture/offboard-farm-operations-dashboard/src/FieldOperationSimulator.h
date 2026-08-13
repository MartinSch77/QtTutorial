// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

namespace qttutorial::agriculture::ops {

struct FieldSample {
    QString fieldId;
    QString vehicleId;
    int passNumber = 1;
    double coveragePercent = 0.0;
    QString status;
};

// Stands in for a farm-management telemetry-ingestion service: given a field
// index and an elapsed time, produces a deterministic, physically plausible
// sample. Each field is phase-shifted so they do not progress in lockstep, but
// every field still follows the same believable pattern: a short idle/downtime
// window at the start of each pass, coverage advancing steadily while working,
// and short turning windows at each row end.
class FieldOperationSimulator {
public:
    static constexpr double kPassPeriodSeconds = 240.0;
    static constexpr double kIdleFraction = 0.08;
    static constexpr double kTurnZoneFraction = 0.04;

    [[nodiscard]] static FieldSample sampleAt(int fieldIndex, double elapsedSeconds);

private:
    [[nodiscard]] static QString fieldIdFor(int fieldIndex);
    [[nodiscard]] static QString vehicleIdFor(int fieldIndex);
};

} // namespace qttutorial::agriculture::ops
