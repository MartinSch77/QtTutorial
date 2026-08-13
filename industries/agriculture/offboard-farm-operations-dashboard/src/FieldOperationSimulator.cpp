// SPDX-License-Identifier: MIT
#include "FieldOperationSimulator.h"

#include <algorithm>
#include <cmath>

namespace qttutorial::agriculture::ops {

QString FieldOperationSimulator::fieldIdFor(int fieldIndex)
{
    return QStringLiteral("FLD-%1").arg(fieldIndex + 1, 3, 10, QLatin1Char('0'));
}

QString FieldOperationSimulator::vehicleIdFor(int fieldIndex)
{
    return QStringLiteral("TRC-%1").arg(fieldIndex + 1, 3, 10, QLatin1Char('0'));
}

double FieldOperationSimulator::dispatchDelaySecondsFor(int fieldIndex)
{
    // Fields are dispatched in sequence, as a real crew would send machines to
    // different fields one after another rather than starting every field at
    // once.
    return static_cast<double>(fieldIndex) * kDispatchDelaySecondsPerField;
}

int FieldOperationSimulator::plannedPassesFor(int fieldIndex)
{
    // Standing in for differing field sizes: not every field needs the same
    // number of passes to finish.
    return 2 + (fieldIndex % 3);
}

double FieldOperationSimulator::engineLoadForStatus(const QString& status)
{
    if (status == QStringLiteral("working")) {
        return kEngineLoadWorkingPercent;
    }
    if (status == QStringLiteral("turning")) {
        return kEngineLoadTurningPercent;
    }
    return kEngineLoadIdlePercent;
}

double FieldOperationSimulator::fuelBurnPercentPerSecondAt(double engineLoadPercent)
{
    return kFuelBurnBaselinePercentPerSecond + engineLoadPercent * kFuelBurnPercentPerSecondPerLoadPoint;
}

double FieldOperationSimulator::averageBurnPercentPerSecond()
{
    // A pass spends a known fraction of its time idle, turning and working
    // (see sampleAt()); blend the burn rate for each phase by that fraction to
    // get a single deterministic average burn rate. This keeps
    // fuelLevelPercent a pure, reproducible function of elapsed time (matching
    // the rest of this simulator's "no independent per-tick noise" style)
    // while still correlating fuel burn with engine load rather than treating
    // it as an unrelated constant.
    const double workingSecondsPerPass = kPassPeriodSeconds * (1.0 - kIdleFraction) * (1.0 - 2.0 * kTurnZoneFraction);
    const double turningSecondsPerPass = kPassPeriodSeconds * (1.0 - kIdleFraction) * (2.0 * kTurnZoneFraction);
    const double idleSecondsPerPass = kPassPeriodSeconds - workingSecondsPerPass - turningSecondsPerPass;

    return (workingSecondsPerPass * fuelBurnPercentPerSecondAt(kEngineLoadWorkingPercent)
            + turningSecondsPerPass * fuelBurnPercentPerSecondAt(kEngineLoadTurningPercent)
            + idleSecondsPerPass * fuelBurnPercentPerSecondAt(kEngineLoadIdlePercent))
        / kPassPeriodSeconds;
}

FieldSample FieldOperationSimulator::sampleAt(int fieldIndex, double elapsedSeconds)
{
    FieldSample sample;
    sample.fieldId = fieldIdFor(fieldIndex);
    sample.vehicleId = vehicleIdFor(fieldIndex);

    const int totalPasses = plannedPassesFor(fieldIndex);
    sample.plannedPasses = totalPasses;

    const double dispatchDelay = dispatchDelaySecondsFor(fieldIndex);

    if (elapsedSeconds < dispatchDelay) {
        sample.passNumber = 0;
        sample.coveragePercent = 0.0;
        sample.status = QStringLiteral("scheduled");
        sample.overallStatus = QStringLiteral("not_started");
        sample.overallFieldProgressPercent = 0.0;
        sample.engineLoadPercent = engineLoadForStatus(sample.status);
        sample.fuelLevelPercent = 100.0;
        return sample;
    }

    const double workElapsed = elapsedSeconds - dispatchDelay;
    const double totalWorkSeconds = static_cast<double>(totalPasses) * kPassPeriodSeconds;
    const double avgBurnPercentPerSecond = averageBurnPercentPerSecond();

    if (workElapsed >= totalWorkSeconds) {
        sample.passNumber = totalPasses;
        sample.coveragePercent = 100.0;
        sample.status = QStringLiteral("complete");
        sample.overallStatus = QStringLiteral("complete");
        sample.overallFieldProgressPercent = 100.0;
        sample.engineLoadPercent = engineLoadForStatus(sample.status);
        sample.fuelLevelPercent = std::max(kFuelFloorPercent, 100.0 - totalWorkSeconds * avgBurnPercentPerSecond);
        return sample;
    }

    const double t = std::fmod(workElapsed, kPassPeriodSeconds);
    const int passNumber = 1 + static_cast<int>(std::floor(workElapsed / kPassPeriodSeconds));
    sample.passNumber = passNumber;

    const double periodFraction = t / kPassPeriodSeconds;
    if (periodFraction < kIdleFraction) {
        sample.coveragePercent = 0.0;
        sample.status = QStringLiteral("idle");
    } else {
        const double activeFraction = (periodFraction - kIdleFraction) / (1.0 - kIdleFraction);
        sample.coveragePercent = activeFraction * 100.0;
        if (activeFraction < kTurnZoneFraction || activeFraction > (1.0 - kTurnZoneFraction)) {
            sample.status = QStringLiteral("turning");
        } else {
            sample.status = QStringLiteral("working");
        }
    }

    sample.overallStatus = QStringLiteral("in_progress");
    sample.overallFieldProgressPercent =
        std::min(100.0, ((static_cast<double>(passNumber) - 1.0) * 100.0 + sample.coveragePercent) / totalPasses);
    sample.engineLoadPercent = engineLoadForStatus(sample.status);
    sample.fuelLevelPercent = std::max(kFuelFloorPercent, 100.0 - workElapsed * avgBurnPercentPerSecond);

    return sample;
}

} // namespace qttutorial::agriculture::ops
