// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

namespace qttutorial::two_wheelers::fleet {

enum class RiderStatus { Riding, Idle, Charging, Maintenance };

struct VehicleSample {
    QString id;
    QString location;
    double speedKph = 0.0;
    double batteryPercent = 100.0;
    RiderStatus status = RiderStatus::Idle;
};

// Stands in for a bike-share/delivery-fleet telemetry-ingestion service: given a
// vehicle index and an elapsed time, produces a deterministic, physically
// plausible sample. Each vehicle is phase-shifted so the fleet does not move in
// lockstep, but every vehicle still follows the same believable pattern of
// riding/idle cruising speed, gradual battery depletion, periodic charging
// stops (speed drops to zero and status flips to Charging while battery is
// low), and occasional maintenance windows.
class FleetVehicleSimulator {
public:
    [[nodiscard]] static VehicleSample sampleAt(int vehicleIndex, double elapsedSeconds);

private:
    [[nodiscard]] static double batteryAt(int vehicleIndex, double elapsedSeconds);
    [[nodiscard]] static bool isMaintenanceWindow(int vehicleIndex, double elapsedSeconds);
    [[nodiscard]] static double speedAt(int vehicleIndex, double elapsedSeconds);
    [[nodiscard]] static QString locationFor(int vehicleIndex);
};

[[nodiscard]] QString riderStatusLabel(RiderStatus status);

} // namespace qttutorial::two_wheelers::fleet
