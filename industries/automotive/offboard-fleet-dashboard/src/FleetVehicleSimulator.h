// SPDX-License-Identifier: MIT
#pragma once

#include <QString>
#include <QStringList>

namespace qttutorial::automotive::fleet {

struct VehicleSample {
    QString id;
    QString location;
    double speedKph = 0.0;
    double fuelPercent = 100.0;
    QStringList faultCodes;
    double efficiencyPercent = 100.0; // 0-100, how close to ideal cruising speed the vehicle is running
    double odometerKm = 0.0;
    bool maintenanceDue = false;
};

// Stands in for a telemetry-ingestion service: given a vehicle index and an
// elapsed time, produces a deterministic, physically plausible sample. Each
// vehicle is phase-shifted so the fleet does not move in lockstep, but every
// vehicle still follows the same believable pattern of cruising speed
// variation and gradual fuel depletion with wraparound (so the demo does not
// need to run for hours to see a refuel event).
//
// efficiencyPercent and odometerKm/maintenanceDue are both derived from the same
// speed signal used for speedAt(), the same "single source of truth" correlation
// convention as speed/fuel: a vehicle running far from the fleet's ideal cruising
// speed scores lower on efficiency, and the odometer accumulates from the average
// of that same speed profile, driving a periodic maintenance-due window (wrapping
// around, like the fuel gauge, so the demo reaches it without waiting for real
// distance to accrue).
class FleetVehicleSimulator {
public:
    static constexpr double kIdealCruiseKph = 60.0;
    static constexpr double kMaintenanceIntervalKm = 500.0;
    static constexpr double kMaintenanceWindowKm = 40.0; // "due soon" window before the interval wraps

    [[nodiscard]] static VehicleSample sampleAt(int vehicleIndex, double elapsedSeconds);

    [[nodiscard]] static double efficiencyAt(double speedKph);
    [[nodiscard]] static double odometerKmAt(int vehicleIndex, double elapsedSeconds);
    [[nodiscard]] static bool isMaintenanceDue(double odometerKm);

private:
    [[nodiscard]] static double speedAt(int vehicleIndex, double elapsedSeconds);
    [[nodiscard]] static double fuelAt(int vehicleIndex, double elapsedSeconds);
    [[nodiscard]] static QString locationFor(int vehicleIndex);
    [[nodiscard]] static QStringList faultCodesFor(int vehicleIndex, double elapsedSeconds, double fuelPercent);
};

} // namespace qttutorial::automotive::fleet
