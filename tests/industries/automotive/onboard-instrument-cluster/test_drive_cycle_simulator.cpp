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

    void ecoModeRespondsMoreGentlyThanSportMode()
    {
        QVERIFY(DriveCycleSimulator::speedTimeConstantSecondsForMode(DrivingMode::Eco)
                > DriveCycleSimulator::speedTimeConstantSecondsForMode(DrivingMode::Sport));
    }

    void sportModeBurnsFuelFasterThanEcoMode()
    {
        QVERIFY(DriveCycleSimulator::fuelRateMultiplierForMode(DrivingMode::Sport)
                > DriveCycleSimulator::fuelRateMultiplierForMode(DrivingMode::Eco));
    }

    void ecoModeIsMoreEfficientThanSportModeAtTheSameAcceleration()
    {
        QVERIFY(DriveCycleSimulator::efficiencyPercentFor(DrivingMode::Eco, 1.0)
                > DriveCycleSimulator::efficiencyPercentFor(DrivingMode::Sport, 1.0));
    }

    void hardAccelerationHurtsEfficiencyRegardlessOfMode()
    {
        const double gentle = DriveCycleSimulator::efficiencyPercentFor(DrivingMode::Eco, 0.5);
        const double hard = DriveCycleSimulator::efficiencyPercentFor(DrivingMode::Eco, 8.0);
        QVERIFY(hard < gentle);
    }

    void settingDrivingModeAffectsSubsequentFuelConsumption()
    {
        DriveCycleSimulator ecoSim;
        ecoSim.setDrivingMode(DrivingMode::Eco);
        DriveCycleSimulator sportSim;
        sportSim.setDrivingMode(DrivingMode::Sport);

        double ecoFuel = 100.0;
        double sportFuel = 100.0;
        for (int i = 0; i < 400; ++i) {
            ecoFuel = ecoSim.advance(0.05).fuelLevelPercent;
            sportFuel = sportSim.advance(0.05).fuelLevelPercent;
        }
        QVERIFY(ecoFuel > sportFuel);
        QCOMPARE(static_cast<int>(ecoSim.state().drivingMode), static_cast<int>(DrivingMode::Eco));
    }

    void tirePressureDriftsDownwardsOverTime()
    {
        const double early = DriveCycleSimulator::tirePressureKpaAt(0, 10.0);
        const double later = DriveCycleSimulator::tirePressureKpaAt(0, 600.0);
        QVERIFY(later < early);
    }

    void frontLeftWheelLeaksFasterAndTriggersWarningEventually()
    {
        bool warningSeen = false;
        for (double t = 0.0; t < 3000.0; t += 30.0) {
            if (DriveCycleSimulator::isTirePressureLow(DriveCycleSimulator::tirePressureKpaAt(0, t))) {
                warningSeen = true;
                break;
            }
        }
        QVERIFY(warningSeen);
    }

    void advanceSurfacesTirePressureWarningOnState()
    {
        DriveCycleSimulator sim;
        bool warningSeen = false;
        for (int i = 0; i < 90000 && !warningSeen; ++i) {
            const VehicleState state = sim.advance(0.05);
            warningSeen = state.tirePressureWarning;
        }
        QVERIFY(warningSeen);
        QVERIFY(sim.state().lowTireWheelIndex >= 0);
    }
};

QTEST_MAIN(TestDriveCycleSimulator)
#include "test_drive_cycle_simulator.moc"
