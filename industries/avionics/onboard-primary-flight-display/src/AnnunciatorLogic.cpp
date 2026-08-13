// SPDX-License-Identifier: MIT
#include "AnnunciatorLogic.h"

#include <cmath>

namespace qttutorial::avionics {

std::vector<CautionMessage> AnnunciatorLogic::evaluate(const FlightState& state) const
{
    std::vector<CautionMessage> messages;

    if (std::abs(state.rollDeg) > kBankAngleCautionDeg) {
        messages.push_back({"BANK ANGLE", "Bank angle exceeds normal operating limit", CautionSeverity::Warning});
    }

    if (state.airspeedKt > kOverspeedCautionKt) {
        messages.push_back({"OVERSPEED", "Airspeed exceeds normal operating limit", CautionSeverity::Warning});
    }

    if (state.airspeedKt < kLowSpeedCautionKt) {
        messages.push_back({"LOW SPEED", "Airspeed approaching low-energy condition", CautionSeverity::Warning});
    }

    if (state.verticalSpeedFtPerMin < kSinkRateCautionFtPerMin && state.altitudeFt < kSinkRateAltitudeCeilingFt) {
        messages.push_back({"SINK RATE", "High descent rate at low altitude", CautionSeverity::Warning});
    }

    if (state.engineTempC > FlightDynamics::kEngineTempCautionC) {
        messages.push_back({"ENGINE", "Engine temperature above normal band", CautionSeverity::Caution});
    }

    return messages;
}

} // namespace qttutorial::avionics
