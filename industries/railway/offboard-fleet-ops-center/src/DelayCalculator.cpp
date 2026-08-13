// SPDX-License-Identifier: MIT
#include "DelayCalculator.h"

namespace qttutorial::fleet_ops {

Punctuality classifyDelay(double delayMinutes)
{
    if (delayMinutes < -1.0) {
        return Punctuality::Early;
    }
    if (delayMinutes <= 1.0) {
        return Punctuality::OnTime;
    }
    if (delayMinutes <= 5.0) {
        return Punctuality::MinorDelay;
    }
    return Punctuality::MajorDelay;
}

const char* punctualityLabel(Punctuality punctuality)
{
    switch (punctuality) {
    case Punctuality::Early:
        return "Early";
    case Punctuality::OnTime:
        return "On time";
    case Punctuality::MinorDelay:
        return "Minor delay";
    case Punctuality::MajorDelay:
        return "Major delay";
    }
    return "Unknown";
}

} // namespace qttutorial::fleet_ops
