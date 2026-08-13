// SPDX-License-Identifier: GPL-3.0-or-later
#include "TelemetryGenerator.h"

#include <array>
#include <cmath>

namespace qttutorial::graphs_showcase {

namespace {

struct SensorSpec {
    const char* name;
    double baseline;
    double amplitude;
    double angularFrequency;
    double phase;
};

constexpr std::array<SensorSpec, 3> kSensorSpecs{{
    {"Sensor A – Pressure", 50.0, 30.0, 0.6, 0.0},
    {"Sensor B – Vibration", 40.0, 25.0, 1.3, 1.0},
    {"Sensor C – Thermal", 60.0, 20.0, 0.4, 2.1},
}};

} // namespace

std::vector<QString> TelemetryGenerator::sensorNames()
{
    std::vector<QString> names;
    names.reserve(kSensorSpecs.size());
    for (const auto& spec : kSensorSpecs) {
        names.push_back(QString::fromUtf8(spec.name));
    }
    return names;
}

double TelemetryGenerator::valueAt(int sensorIndex, double elapsedSeconds)
{
    if (sensorIndex < 0 || static_cast<std::size_t>(sensorIndex) >= kSensorSpecs.size()) {
        return 0.0;
    }
    const auto& spec = kSensorSpecs[static_cast<std::size_t>(sensorIndex)];
    return spec.baseline
        + spec.amplitude * std::sin(spec.phase + elapsedSeconds * spec.angularFrequency);
}

} // namespace qttutorial::graphs_showcase
