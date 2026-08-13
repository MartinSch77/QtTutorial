// SPDX-License-Identifier: MIT
#include "FleetVehicleSimulator.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>

namespace qttutorial::automotive::fleet {

namespace {
constexpr double kTwoPi = 2.0 * std::numbers::pi;

const std::array<const char*, 6> kLocations = {
    "Depot A",       "Route 9 - mile 12",     "Warehouse District",
    "Port Terminal", "Route 66 - mile 45",    "Depot B",
};
} // namespace

double FleetVehicleSimulator::speedAt(int vehicleIndex, double elapsedSeconds)
{
    const double phaseOffset = static_cast<double>(vehicleIndex) * 7.0;
    const double cruise = 60.0 + 30.0 * std::sin(kTwoPi * (elapsedSeconds + phaseOffset) / 90.0);
    return std::max(0.0, cruise);
}

double FleetVehicleSimulator::fuelAt(int vehicleIndex, double elapsedSeconds)
{
    const double offset = static_cast<double>(vehicleIndex) * 13.0;
    const double drained = std::fmod(elapsedSeconds * 0.05 + offset, 100.0);
    return 100.0 - drained;
}

QString FleetVehicleSimulator::locationFor(int vehicleIndex)
{
    return QString::fromLatin1(kLocations[static_cast<std::size_t>(vehicleIndex) % kLocations.size()]);
}

QStringList FleetVehicleSimulator::faultCodesFor(int vehicleIndex, double elapsedSeconds, double fuelPercent)
{
    QStringList codes;
    if (fuelPercent < 15.0) {
        codes << QStringLiteral("F-FUEL-LOW");
    }
    const auto window = static_cast<long long>(elapsedSeconds / 30.0);
    if ((vehicleIndex + window) % 7 == 0) {
        codes << QStringLiteral("F-ENGINE-CHECK");
    }
    return codes;
}

VehicleSample FleetVehicleSimulator::sampleAt(int vehicleIndex, double elapsedSeconds)
{
    VehicleSample sample;
    sample.id = QStringLiteral("VH-%1").arg(vehicleIndex + 1, 3, 10, QLatin1Char('0'));
    sample.location = locationFor(vehicleIndex);
    sample.speedKph = speedAt(vehicleIndex, elapsedSeconds);
    sample.fuelPercent = fuelAt(vehicleIndex, elapsedSeconds);
    sample.faultCodes = faultCodesFor(vehicleIndex, elapsedSeconds, sample.fuelPercent);
    return sample;
}

} // namespace qttutorial::automotive::fleet
