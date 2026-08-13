// SPDX-License-Identifier: MIT
#include "VehicleStatusSimulator.h"

#include <QTest>

using namespace qttutorial::defence;

class TestVehicleStatusSimulator : public QObject {
    Q_OBJECT
private slots:
    void startsWithFullFuel()
    {
        VehicleStatusSimulator sim;
        QCOMPARE(sim.status().fuelPercent, 100.0);
    }

    void headingStaysInRange()
    {
        VehicleStatusSimulator sim;
        for (int i = 0; i < 500; ++i) {
            sim.advance(1.0);
            QVERIFY(sim.status().headingDeg >= 0.0);
            QVERIFY(sim.status().headingDeg < 360.0);
        }
    }

    void fuelDecreasesOverTimeAndNeverGoesNegative()
    {
        VehicleStatusSimulator sim;
        const double initialFuel = sim.status().fuelPercent;
        sim.advance(60.0);
        QVERIFY(sim.status().fuelPercent < initialFuel);

        for (int i = 0; i < 100000; ++i) {
            sim.advance(60.0);
        }
        QVERIFY(sim.status().fuelPercent >= 0.0);
    }

    void speedStaysWithinPlausibleBand()
    {
        VehicleStatusSimulator sim;
        for (int i = 0; i < 500; ++i) {
            sim.advance(1.0);
            QVERIFY(sim.status().speedUnits >= 0.0);
            QVERIFY(sim.status().speedUnits <= 40.0);
        }
    }
};

QTEST_MAIN(TestVehicleStatusSimulator)
#include "test_vehicle_status_simulator.moc"
