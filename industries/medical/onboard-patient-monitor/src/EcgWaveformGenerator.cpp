// SPDX-License-Identifier: MIT
#include "EcgWaveformGenerator.h"

#include <cmath>

namespace qttutorial::medical {

namespace {

double gaussianBump(double phase, double center, double width, double amplitude)
{
    const double d = phase - center;
    return amplitude * std::exp(-(d * d) / (2.0 * width * width));
}

} // namespace

double EcgWaveformGenerator::sample(double timeSeconds, double heartRateBpm)
{
    const double cycleLength = 60.0 / heartRateBpm;
    const double phase = std::fmod(timeSeconds, cycleLength) / cycleLength;

    double value = 0.0;
    value += gaussianBump(phase, 0.15, 0.035, 0.15);   // P wave
    value += gaussianBump(phase, 0.28, 0.012, -0.15);  // Q dip
    value += gaussianBump(phase, 0.30, 0.010, 1.0);    // R spike
    value += gaussianBump(phase, 0.32, 0.012, -0.25);  // S dip
    value += gaussianBump(phase, 0.55, 0.08, 0.3);     // T wave
    return value;
}

} // namespace qttutorial::medical
