// SPDX-License-Identifier: MIT
#include "DriveCycleSimulator.h"

#include <QTest>

using namespace qttutorial::automotive;

class TestDriveCycleSimulator : public QObject {
    Q_OBJECT
private slots:
    void acceleratesFromStandstill()
    {
        DriveCycleSimulator sim;
        double lastSpeed = 0.0;
        for (int i = 0; i < 100; ++i) {
            const VehicleState state = sim.advance(0.05);
            QVERIFY(state.speedKph >= lastSpeed - 1e-9);
            lastSpeed = state.speedKph;
        }
        QVERIFY(lastSpeed > 15.0);
    }

    void gearIncreasesWithSpeed()
    {
        QCOMPARE(DriveCycleSimulator::gearForSpeed(0.0), Gear::Park);
        QCOMPARE(DriveCycleSimulator::gearForSpeed(10.0), Gear::Drive1);
        QCOMPARE(DriveCycleSimulator::gearForSpeed(30.0), Gear::Drive2);
        QCOMPARE(DriveCycleSimulator::gearForSpeed(110.0), Gear::Drive6);
        QCOMPARE(DriveCycleSimulator::gearForSpeed(-5.0), Gear::Reverse);
    }

    void rpmCorrelatesWithSpeedWithinGear()
    {
        const double lowRpm = DriveCycleSimulator::rpmForSpeedAndGear(21.0, Gear::Drive1);
        const double highRpm = DriveCycleSimulator::rpmForSpeedAndGear(39.0, Gear::Drive1);
        QVERIFY(highRpm > lowRpm);
    }

    void idleRpmAtStandstill()
    {
        QCOMPARE(DriveCycleSimulator::rpmForSpeedAndGear(0.0, Gear::Park), 800.0);
    }

    void fuelDepletesWithDistanceTravelled()
    {
        DriveCycleSimulator sim;
        VehicleState first = sim.advance(0.05);
        for (int i = 0; i < 400; ++i) {
            sim.advance(0.05);
        }
        const VehicleState later = sim.state();
        QVERIFY(later.fuelLevelPercent < first.fuelLevelPercent);
    }

    void lowFuelWarningThreshold()
    {
        QVERIFY(DriveCycleSimulator::isLowFuel(10.0));
        QVERIFY(!DriveCycleSimulator::isLowFuel(50.0));
    }

    void followingDistanceCollapsesDuringBrakingPhase()
    {
        QCOMPARE(DriveCycleSimulator::followingDistanceAt(25.0), 3);
        QVERIFY(DriveCycleSimulator::followingDistanceAt(35.0) < 3);
    }
};

QTEST_MAIN(TestDriveCycleSimulator)
#include "test_drive_cycle_simulator.moc"
