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
    double odometerKm = 0.0;
    bool maintenanceDue = false;
};

// Stands in for a bike-share/delivery-fleet telemetry-ingestion service: given a
// vehicle index and an elapsed time, produces a deterministic, physically
// plausible sample. Each vehicle is phase-shifted so the fleet does not move in
// lockstep, but every vehicle still follows the same believable pattern of
// riding/idle cruising speed, gradual battery depletion, periodic charging
// stops (speed drops to zero and status flips to Charging while battery is
// low), and occasional maintenance windows.
//
// Odometer/maintenance-due modelling: each vehicle accrues distance at a
// deterministic, per-vehicle usage rate (busier vehicle indices simply cover
// more ground per hour - a coarse stand-in for "some bikes in a fleet just get
// ridden more"), so odometerKm increases monotonically with elapsed time and is
// distinct per vehicle. A vehicle is flagged maintenanceDue for the last stretch
// of km before it crosses each multiple of a fixed service interval, the way a
// real fleet-management system raises a "service due soon" flag from mileage
// alone - this is intentionally independent of the short-lived isMaintenanceWindow
// "currently being serviced" status below.
class FleetVehicleSimulator {
public:
    static constexpr double kServiceIntervalKm = 3000.0;
    static constexpr double kMaintenanceDueWindowKm = 250.0;

    [[nodiscard]] static VehicleSample sampleAt(int vehicleIndex, double elapsedSeconds);

    [[nodiscard]] static double odometerKmAt(int vehicleIndex, double elapsedSeconds);
    [[nodiscard]] static bool maintenanceDueAt(int vehicleIndex, double elapsedSeconds);
    [[nodiscard]] static double usageRateKphForVehicle(int vehicleIndex);

private:
    [[nodiscard]] static double batteryAt(int vehicleIndex, double elapsedSeconds);
    [[nodiscard]] static bool isMaintenanceWindow(int vehicleIndex, double elapsedSeconds);
    [[nodiscard]] static double speedAt(int vehicleIndex, double elapsedSeconds);
    [[nodiscard]] static QString locationFor(int vehicleIndex);
};

[[nodiscard]] QString riderStatusLabel(RiderStatus status);

} // namespace qttutorial::two_wheelers::fleet
