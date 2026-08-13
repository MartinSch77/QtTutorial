// SPDX-License-Identifier: MIT
#include "FieldOperationSimulator.h"

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

FieldSample FieldOperationSimulator::sampleAt(int fieldIndex, double elapsedSeconds)
{
    const double phaseOffset = static_cast<double>(fieldIndex) * 53.0;
    const double shiftedSeconds = elapsedSeconds + phaseOffset;
    const double t = std::fmod(shiftedSeconds, kPassPeriodSeconds);
    const int passNumber = 1 + static_cast<int>(std::floor(shiftedSeconds / kPassPeriodSeconds));

    FieldSample sample;
    sample.fieldId = fieldIdFor(fieldIndex);
    sample.vehicleId = vehicleIdFor(fieldIndex);
    sample.passNumber = passNumber;

    const double periodFraction = t / kPassPeriodSeconds;
    if (periodFraction < kIdleFraction) {
        sample.coveragePercent = 0.0;
        sample.status = QStringLiteral("idle");
        return sample;
    }

    const double activeFraction = (periodFraction - kIdleFraction) / (1.0 - kIdleFraction);
    sample.coveragePercent = activeFraction * 100.0;
    if (activeFraction < kTurnZoneFraction || activeFraction > (1.0 - kTurnZoneFraction)) {
        sample.status = QStringLiteral("turning");
    } else {
        sample.status = QStringLiteral("working");
    }
    return sample;
}

} // namespace qttutorial::agriculture::ops
