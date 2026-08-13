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
};

// Stands in for a telemetry-ingestion service: given a vehicle index and an
// elapsed time, produces a deterministic, physically plausible sample. Each
// vehicle is phase-shifted so the fleet does not move in lockstep, but every
// vehicle still follows the same believable pattern of cruising speed
// variation and gradual fuel depletion with wraparound (so the demo does not
// need to run for hours to see a refuel event).
class FleetVehicleSimulator {
public:
    [[nodiscard]] static VehicleSample sampleAt(int vehicleIndex, double elapsedSeconds);

private:
    [[nodiscard]] static double speedAt(int vehicleIndex, double elapsedSeconds);
    [[nodiscard]] static double fuelAt(int vehicleIndex, double elapsedSeconds);
    [[nodiscard]] static QString locationFor(int vehicleIndex);
    [[nodiscard]] static QStringList faultCodesFor(int vehicleIndex, double elapsedSeconds, double fuelPercent);
};

} // namespace qttutorial::automotive::fleet
