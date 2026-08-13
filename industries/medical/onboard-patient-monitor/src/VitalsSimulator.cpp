// SPDX-License-Identifier: MIT
#include "VitalsSimulator.h"

#include <cmath>
#include <numbers>

namespace qttutorial::medical {

namespace {
constexpr double kTwoPi = 2.0 * std::numbers::pi;
}

double VitalsSimulator::episodeSeverityAt(double timeSeconds)
{
    const double phase = std::fmod(timeSeconds, kEpisodePeriodSeconds);
    if (phase < 40.0 || phase >= 75.0) {
        return 0.0;
    }
    if (phase < 50.0) {
        return (phase - 40.0) / 10.0;
    }
    if (phase < 65.0) {
        return 1.0;
    }
    return 1.0 - (phase - 65.0) / 10.0;
}

double VitalsSimulator::heartRateAt(double timeSeconds)
{
    const double baseline = 72.0 + 5.0 * std::sin(kTwoPi * timeSeconds / 12.0);
    return baseline + episodeSeverityAt(timeSeconds) * 55.0;
}

double VitalsSimulator::spo2At(double timeSeconds)
{
    const double baseline = 97.0 + 1.0 * std::sin(kTwoPi * timeSeconds / 50.0);
    return baseline - episodeSeverityAt(timeSeconds) * 9.0;
}

double VitalsSimulator::systolicAt(double timeSeconds)
{
    const double baseline = 118.0 + 6.0 * std::sin(kTwoPi * timeSeconds / 70.0);
    return baseline + episodeSeverityAt(timeSeconds) * 10.0;
}

double VitalsSimulator::diastolicAt(double timeSeconds)
{
    return systolicAt(timeSeconds) * 0.64;
}

} // namespace qttutorial::medical
