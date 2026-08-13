// SPDX-License-Identifier: MIT
#include "IndicatorGenerator.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace qttutorial::qml_basics {

namespace {

struct IndicatorSpec {
    const char* name;
    double baseline;
    double amplitude;
    double phase;
};

constexpr std::array<IndicatorSpec, 5> kIndicatorSpecs{{
    {"CPU Load", 45.0, 35.0, 0.0},
    {"Memory", 55.0, 20.0, 0.7},
    {"Network I/O", 30.0, 45.0, 1.4},
    {"Disk I/O", 25.0, 20.0, 2.1},
    {"Temperature", 50.0, 25.0, 2.8},
}};

constexpr double kWarningThreshold = 65.0;
constexpr double kCriticalThreshold = 85.0;

} // namespace

IndicatorStatus IndicatorGenerator::statusFor(double value, double warningThreshold,
                                                double criticalThreshold)
{
    if (value >= criticalThreshold) {
        return IndicatorStatus::Critical;
    }
    if (value >= warningThreshold) {
        return IndicatorStatus::Warning;
    }
    return IndicatorStatus::Nominal;
}

std::vector<QString> IndicatorGenerator::indicatorNames()
{
    std::vector<QString> names;
    names.reserve(kIndicatorSpecs.size());
    for (const auto& spec : kIndicatorSpecs) {
        names.push_back(QString::fromUtf8(spec.name));
    }
    return names;
}

std::vector<IndicatorReading> IndicatorGenerator::generate(int tick)
{
    std::vector<IndicatorReading> readings;
    readings.reserve(kIndicatorSpecs.size());
    for (const auto& spec : kIndicatorSpecs) {
        const double value = std::clamp(
            spec.baseline + spec.amplitude * std::sin(spec.phase + static_cast<double>(tick) * 0.3),
            0.0, 100.0);
        readings.push_back({QString::fromUtf8(spec.name), value,
                             statusFor(value, kWarningThreshold, kCriticalThreshold)});
    }
    return readings;
}

} // namespace qttutorial::qml_basics
