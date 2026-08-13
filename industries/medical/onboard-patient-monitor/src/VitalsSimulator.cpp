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

double VitalsSimulator::feverSeverityAt(double timeSeconds)
{
    const double phase = std::fmod(timeSeconds, kFeverPeriodSeconds);
    if (phase < 100.0 || phase >= 140.0) {
        return 0.0;
    }
    if (phase < 110.0) {
        return (phase - 100.0) / 10.0;
    }
    if (phase < 125.0) {
        return 1.0;
    }
    return 1.0 - (phase - 125.0) / 15.0;
}

double VitalsSimulator::heartRateAt(double timeSeconds)
{
    const double baseline = 72.0 + 5.0 * std::sin(kTwoPi * timeSeconds / 12.0);
    // Desaturation events cause a sharp tachycardia; fever causes a milder
    // one. Both may in principle coincide (their periods are deliberately
    // different lengths so they drift relative to each other), and simply
    // adding the two contributions is a reasonable approximation.
    return baseline + episodeSeverityAt(timeSeconds) * 55.0 + feverSeverityAt(timeSeconds) * 15.0;
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

double VitalsSimulator::respirationRateAt(double timeSeconds)
{
    const double baseline = 16.0 + 1.5 * std::sin(kTwoPi * timeSeconds / 20.0);
    // A desaturation event plausibly correlates with faster, more laboured
    // breathing; a fever contributes a smaller respiratory-rate rise too.
    return baseline + episodeSeverityAt(timeSeconds) * 10.0 + feverSeverityAt(timeSeconds) * 4.0;
}

double VitalsSimulator::temperatureAt(double timeSeconds)
{
    const double baseline = 37.0 + 0.15 * std::sin(kTwoPi * timeSeconds / 300.0);
    // Fever correlates elevated temperature with the heart-rate/respiration
    // rise modelled above; a desaturation event alone does not raise
    // temperature.
    return baseline + feverSeverityAt(timeSeconds) * 1.7;
}

} // namespace qttutorial::medical
