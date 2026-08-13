// SPDX-License-Identifier: MIT
#include "FleetVehicleSimulator.h"

#include <QTest>

using namespace qttutorial::automotive::fleet;

class TestFleetVehicleSimulator : public QObject {
    Q_OBJECT
private slots:
    void producesDistinctIdsAndLocations()
    {
        const VehicleSample a = FleetVehicleSimulator::sampleAt(0, 0.0);
        const VehicleSample b = FleetVehicleSimulator::sampleAt(1, 0.0);
        QVERIFY(a.id != b.id);
        QCOMPARE(a.id, QStringLiteral("VH-001"));
    }

    void speedStaysNonNegative()
    {
        for (int i = 0; i < 8; ++i) {
            for (double t = 0.0; t < 200.0; t += 5.0) {
                QVERIFY(FleetVehicleSimulator::sampleAt(i, t).speedKph >= 0.0);
            }
        }
    }

    void fuelWrapsAroundInsteadOfGoingNegative()
    {
        const VehicleSample sample = FleetVehicleSimulator::sampleAt(0, 5000.0);
        QVERIFY(sample.fuelPercent >= 0.0);
        QVERIFY(sample.fuelPercent <= 100.0);
    }

    void lowFuelTriggersFaultCode()
    {
        double lowFuelTime = 0.0;
        for (double t = 0.0; t < 2000.0; t += 1.0) {
            if (FleetVehicleSimulator::sampleAt(0, t).fuelPercent < 15.0) {
                lowFuelTime = t;
                break;
            }
        }
        QVERIFY(lowFuelTime > 0.0);
        const VehicleSample sample = FleetVehicleSimulator::sampleAt(0, lowFuelTime);
        QVERIFY(sample.faultCodes.contains(QStringLiteral("F-FUEL-LOW")));
    }

    void efficiencyPeaksAtIdealCruiseSpeed()
    {
        QCOMPARE(FleetVehicleSimulator::efficiencyAt(FleetVehicleSimulator::kIdealCruiseKph), 100.0);
        QVERIFY(FleetVehicleSimulator::efficiencyAt(FleetVehicleSimulator::kIdealCruiseKph + 20.0) < 100.0);
        QVERIFY(FleetVehicleSimulator::efficiencyAt(FleetVehicleSimulator::kIdealCruiseKph - 20.0) < 100.0);
    }

    void efficiencyIsDerivedFromSampleSpeed()
    {
        const VehicleSample sample = FleetVehicleSimulator::sampleAt(0, 0.0);
        QCOMPARE(sample.efficiencyPercent, FleetVehicleSimulator::efficiencyAt(sample.speedKph));
    }

    void odometerIncreasesMonotonicallyOverTime()
    {
        const double early = FleetVehicleSimulator::odometerKmAt(0, 100.0);
        const double later = FleetVehicleSimulator::odometerKmAt(0, 10000.0);
        QVERIFY(later > early);
    }

    void odometerMatchesSampleAndStaysNonNegative()
    {
        for (double t = 0.0; t < 5000.0; t += 250.0) {
            const VehicleSample sample = FleetVehicleSimulator::sampleAt(2, t);
            QCOMPARE(sample.odometerKm, FleetVehicleSimulator::odometerKmAt(2, t));
            QVERIFY(sample.odometerKm >= 0.0);
        }
    }

    void maintenanceBecomesDueAsIntervalIsApproachedAndWrapsAround()
    {
        QVERIFY(!FleetVehicleSimulator::isMaintenanceDue(0.0));
        QVERIFY(FleetVehicleSimulator::isMaintenanceDue(FleetVehicleSimulator::kMaintenanceIntervalKm - 1.0));
        // After wrapping past the interval, a fresh service window starts again.
        QVERIFY(!FleetVehicleSimulator::isMaintenanceDue(FleetVehicleSimulator::kMaintenanceIntervalKm + 1.0));
    }

    void maintenanceDueEventuallyTriggersForARunningFleetVehicle()
    {
        bool dueSeen = false;
        for (double t = 0.0; t < 200000.0; t += 500.0) {
            if (FleetVehicleSimulator::sampleAt(1, t).maintenanceDue) {
                dueSeen = true;
                break;
            }
        }
        QVERIFY(dueSeen);
    }
};

QTEST_MAIN(TestFleetVehicleSimulator)
#include "test_fleet_vehicle_simulator.moc"
