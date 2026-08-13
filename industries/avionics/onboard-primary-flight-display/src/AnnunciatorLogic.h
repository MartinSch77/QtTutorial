// SPDX-License-Identifier: MIT
#pragma once

#include "FlightDynamics.h"

#include <string>
#include <vector>

namespace qttutorial::avionics {

enum class CautionSeverity { Caution, Warning };

struct CautionMessage {
    std::string id;   // short annunciator legend, e.g. "BANK ANGLE"
    std::string text; // human-readable description for a tooltip/log
    CautionSeverity severity = CautionSeverity::Caution;
};

// Pure C++23 evaluation of a FlightState against a small set of avionics-style
// caution/warning thresholds. This is deliberately *not* independent random
// jitter: every condition here is derived from the same coupled flight-dynamics
// state that drives the rest of the PFD, so a caution lighting up is a
// consequence of what the aircraft is actually doing (a steep bank, a low-energy
// approach-to-stall condition, a high sink rate close to the ground, or an
// engine running hot after a sustained high-power climb) rather than a scripted
// light-flicker demo.
class AnnunciatorLogic {
public:
    static constexpr double kBankAngleCautionDeg = 28.0;
    static constexpr double kOverspeedCautionKt = 350.0;
    static constexpr double kLowSpeedCautionKt = 100.0;
    static constexpr double kSinkRateCautionFtPerMin = -1800.0;
    static constexpr double kSinkRateAltitudeCeilingFt = 2500.0;

    [[nodiscard]] std::vector<CautionMessage> evaluate(const FlightState& state) const;
};

} // namespace qttutorial::avionics
