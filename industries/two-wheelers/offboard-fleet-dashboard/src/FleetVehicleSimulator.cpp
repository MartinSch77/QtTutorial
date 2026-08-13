// SPDX-License-Identifier: MIT
#include "FleetVehicleSimulator.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>

namespace qttutorial::two_wheelers::fleet {

namespace {
constexpr double kTwoPi = 2.0 * std::numbers::pi;
constexpr double kLowBatteryPercent = 15.0;
constexpr double kMaintenanceWindowSeconds = 40.0;
constexpr double kMaintenanceDurationSeconds = 12.0;

const std::array<const char*, 6> kLocations = {
    "Riverside Dock", "Market Street",   "University Loop", "Harbor Front", "Old Town Square", "North Depot",
};
} // namespace

double FleetVehicleSimulator::batteryAt(int vehicleIndex, double elapsedSeconds)
{
    const double offset = static_cast<double>(vehicleIndex) * 13.0;
    const double drained = std::fmod(elapsedSeconds * 0.08 + offset, 100.0);
    return 100.0 - drained;
}

bool FleetVehicleSimulator::isMaintenanceWindow(int vehicleIndex, double elapsedSeconds)
{
    const auto window = static_cast<long long>(elapsedSeconds / kMaintenanceWindowSeconds);
    const double localTime = std::fmod(elapsedSeconds, kMaintenanceWindowSeconds);
    return (vehicleIndex + window) % 9 == 0 && localTime < kMaintenanceDurationSeconds;
}

double FleetVehicleSimulator::speedAt(int vehicleIndex, double elapsedSeconds)
{
    const double phaseOffset = static_cast<double>(vehicleIndex) * 9.0;
    const double cruise = 22.0 + 16.0 * std::sin(kTwoPi * (elapsedSeconds + phaseOffset) / 45.0);
    return std::max(0.0, cruise);
}

QString FleetVehicleSimulator::locationFor(int vehicleIndex)
{
    return QString::fromLatin1(kLocations[static_cast<std::size_t>(vehicleIndex) % kLocations.size()]);
}

double FleetVehicleSimulator::usageRateKphForVehicle(int vehicleIndex)
{
    // Deterministic per-vehicle average usage rate: busier vehicles (higher
    // index modulo 5) rack up distance faster, so the fleet's odometers/
    // maintenance-due flags are spread out rather than moving in lockstep.
    constexpr double kBaseUsageKph = 9.0;
    constexpr double kUsageStepKph = 2.5;
    return kBaseUsageKph + static_cast<double>(vehicleIndex % 5) * kUsageStepKph;
}

double FleetVehicleSimulator::odometerKmAt(int vehicleIndex, double elapsedSeconds)
{
    const double elapsedHours = elapsedSeconds / 3600.0;
    return usageRateKphForVehicle(vehicleIndex) * elapsedHours;
}

bool FleetVehicleSimulator::maintenanceDueAt(int vehicleIndex, double elapsedSeconds)
{
    const double odometer = odometerKmAt(vehicleIndex, elapsedSeconds);
    const double intoInterval = std::fmod(odometer, kServiceIntervalKm);
    return intoInterval >= (kServiceIntervalKm - kMaintenanceDueWindowKm);
}

VehicleSample FleetVehicleSimulator::sampleAt(int vehicleIndex, double elapsedSeconds)
{
    VehicleSample sample;
    sample.id = QStringLiteral("TW-%1").arg(vehicleIndex + 1, 3, 10, QLatin1Char('0'));
    sample.location = locationFor(vehicleIndex);
    sample.batteryPercent = batteryAt(vehicleIndex, elapsedSeconds);
    sample.odometerKm = odometerKmAt(vehicleIndex, elapsedSeconds);
    sample.maintenanceDue = maintenanceDueAt(vehicleIndex, elapsedSeconds);

    if (isMaintenanceWindow(vehicleIndex, elapsedSeconds)) {
        sample.status = RiderStatus::Maintenance;
        sample.speedKph = 0.0;
        return sample;
    }
    if (sample.batteryPercent < kLowBatteryPercent) {
        sample.status = RiderStatus::Charging;
        sample.speedKph = 0.0;
        return sample;
    }

    sample.speedKph = speedAt(vehicleIndex, elapsedSeconds);
    sample.status = sample.speedKph > 3.0 ? RiderStatus::Riding : RiderStatus::Idle;
    return sample;
}

QString riderStatusLabel(RiderStatus status)
{
    switch (status) {
    case RiderStatus::Riding:
        return QStringLiteral("riding");
    case RiderStatus::Idle:
        return QStringLiteral("idle");
    case RiderStatus::Charging:
        return QStringLiteral("charging");
    case RiderStatus::Maintenance:
        return QStringLiteral("maintenance");
    }
    return QStringLiteral("idle");
}

} // namespace qttutorial::two_wheelers::fleet
