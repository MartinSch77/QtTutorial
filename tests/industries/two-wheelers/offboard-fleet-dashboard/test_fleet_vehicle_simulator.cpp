// SPDX-License-Identifier: MIT
#include "FleetVehicleSimulator.h"

#include <QTest>

using namespace qttutorial::two_wheelers::fleet;

class TestFleetVehicleSimulator : public QObject {
    Q_OBJECT
private slots:
    void producesDistinctIdsAndLocations()
    {
        const VehicleSample a = FleetVehicleSimulator::sampleAt(0, 0.0);
        const VehicleSample b = FleetVehicleSimulator::sampleAt(1, 0.0);
        QVERIFY(a.id != b.id);
        QCOMPARE(a.id, QStringLiteral("TW-001"));
    }

    void speedStaysNonNegative()
    {
        for (int i = 0; i < 10; ++i) {
            for (double t = 0.0; t < 400.0; t += 5.0) {
                QVERIFY(FleetVehicleSimulator::sampleAt(i, t).speedKph >= 0.0);
            }
        }
    }

    void batteryStaysWithinBounds()
    {
        for (int i = 0; i < 10; ++i) {
            for (double t = 0.0; t < 2000.0; t += 10.0) {
                const double battery = FleetVehicleSimulator::sampleAt(i, t).batteryPercent;
                QVERIFY(battery >= 0.0);
                QVERIFY(battery <= 100.0);
            }
        }
    }

    void chargingStatusImpliesLowBatteryAndZeroSpeed()
    {
        bool sawCharging = false;
        for (double t = 0.0; t < 2000.0; t += 1.0) {
            const VehicleSample sample = FleetVehicleSimulator::sampleAt(0, t);
            if (sample.status == RiderStatus::Charging) {
                sawCharging = true;
                QVERIFY(sample.batteryPercent < 15.0);
                QCOMPARE(sample.speedKph, 0.0);
            }
        }
        QVERIFY(sawCharging);
    }

    void maintenanceStatusImpliesZeroSpeed()
    {
        bool sawMaintenance = false;
        for (int i = 0; i < 10; ++i) {
            for (double t = 0.0; t < 400.0; t += 1.0) {
                const VehicleSample sample = FleetVehicleSimulator::sampleAt(i, t);
                if (sample.status == RiderStatus::Maintenance) {
                    sawMaintenance = true;
                    QCOMPARE(sample.speedKph, 0.0);
                }
            }
        }
        QVERIFY(sawMaintenance);
    }

    void ridingStatusOnlyWhenMoving()
    {
        for (int i = 0; i < 10; ++i) {
            for (double t = 0.0; t < 400.0; t += 5.0) {
                const VehicleSample sample = FleetVehicleSimulator::sampleAt(i, t);
                if (sample.status == RiderStatus::Riding) {
                    QVERIFY(sample.speedKph > 3.0);
                }
            }
        }
    }

    void riderStatusLabelsAreLowercase()
    {
        QCOMPARE(riderStatusLabel(RiderStatus::Riding), QStringLiteral("riding"));
        QCOMPARE(riderStatusLabel(RiderStatus::Idle), QStringLiteral("idle"));
        QCOMPARE(riderStatusLabel(RiderStatus::Charging), QStringLiteral("charging"));
        QCOMPARE(riderStatusLabel(RiderStatus::Maintenance), QStringLiteral("maintenance"));
    }
};

QTEST_MAIN(TestFleetVehicleSimulator)
#include "test_fleet_vehicle_simulator.moc"
