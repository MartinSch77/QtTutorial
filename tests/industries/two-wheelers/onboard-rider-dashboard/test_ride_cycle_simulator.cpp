// SPDX-License-Identifier: MIT
#include "RideCycleSimulator.h"

#include <QTest>

#include <cmath>

using namespace qttutorial::two_wheelers;

class TestRideCycleSimulator : public QObject {
    Q_OBJECT
private slots:
    void acceleratesFromStandstill()
    {
        RideCycleSimulator sim;
        double lastSpeed = 0.0;
        for (int i = 0; i < 100; ++i) {
            const RideState state = sim.advance(0.05);
            QVERIFY(state.speedKph >= lastSpeed - 1e-9);
            lastSpeed = state.speedKph;
        }
        QVERIFY(lastSpeed > 15.0);
    }

    void gearIncreasesWithSpeed()
    {
        QCOMPARE(RideCycleSimulator::gearForSpeed(0.0), Gear::Neutral);
        QCOMPARE(RideCycleSimulator::gearForSpeed(10.0), Gear::Gear1);
        QCOMPARE(RideCycleSimulator::gearForSpeed(35.0), Gear::Gear3);
        QCOMPARE(RideCycleSimulator::gearForSpeed(80.0), Gear::Gear6);
    }

    void rpmCorrelatesWithSpeedWithinGear()
    {
        const double lowRpm = RideCycleSimulator::rpmForSpeedAndGear(16.0, Gear::Gear1);
        const double highRpm = RideCycleSimulator::rpmForSpeedAndGear(29.0, Gear::Gear1);
        QVERIFY(highRpm > lowRpm);
    }

    void idleRpmInNeutral()
    {
        QCOMPARE(RideCycleSimulator::rpmForSpeedAndGear(0.0, Gear::Neutral), 1200.0);
    }

    void leanAngleIsZeroOutsideCorneringPhase()
    {
        QVERIFY(!RideCycleSimulator::isCorneringPhase(5.0));
        QCOMPARE(RideCycleSimulator::leanAngleAt(5.0, 80.0), 0.0);
        QVERIFY(!RideCycleSimulator::isCorneringPhase(50.0));
        QCOMPARE(RideCycleSimulator::leanAngleAt(50.0, 80.0), 0.0);
    }

    void leanAngleTracksCorneringPhaseShape()
    {
        QVERIFY(RideCycleSimulator::isCorneringPhase(30.0));
        QVERIFY(RideCycleSimulator::isCorneringPhase(37.5));
        QVERIFY(RideCycleSimulator::isCorneringPhase(44.9));

        // Chicane shape: right lean in the first half of the corner, left lean
        // in the second half, and (near) zero at both the midpoint and edges.
        QVERIFY(RideCycleSimulator::leanAngleAt(33.75, 45.0) > 5.0);
        QVERIFY(RideCycleSimulator::leanAngleAt(41.25, 45.0) < -5.0);
        QCOMPARE(RideCycleSimulator::leanAngleAt(30.0, 45.0), 0.0);
    }

    void leanAngleScalesWithSpeedDuringCorner()
    {
        const double slowLean = RideCycleSimulator::leanAngleAt(33.75, 20.0);
        const double fastLean = RideCycleSimulator::leanAngleAt(33.75, 55.0);
        QVERIFY(fastLean > slowLean);
    }

    void leanAngleReturnsToZeroAfterCorneringInSimulation()
    {
        RideCycleSimulator sim;
        for (int i = 0; i < 1400; ++i) {
            sim.advance(0.05);
        }
        // 1400 * 0.05s = 70s, well past the [30, 45) cornering window of the
        // 60s cycle (wrapped once), so lean should have decayed back near zero.
        QVERIFY(std::abs(sim.state().leanAngleDeg) < 2.0);
    }

    void tyreTemperatureRisesWithSustainedSpeed()
    {
        RideCycleSimulator sim;
        for (int i = 0; i < 20; ++i) {
            sim.advance(0.05);
        }
        const double earlyTemp = sim.state().frontTyreTempC;
        for (int i = 0; i < 600; ++i) {
            sim.advance(0.05);
        }
        QVERIFY(sim.state().frontTyreTempC > earlyTemp);
    }

    void rearTyreRunsHotterThanFront()
    {
        RideCycleSimulator sim;
        for (int i = 0; i < 600; ++i) {
            sim.advance(0.05);
        }
        QVERIFY(sim.state().rearTyreTempC >= sim.state().frontTyreTempC);
    }

    void tyrePressureIncreasesWithTemperature()
    {
        const double coolPressure = RideCycleSimulator::tyrePressureForTemp(20.0, false);
        const double hotPressure = RideCycleSimulator::tyrePressureForTemp(60.0, false);
        QVERIFY(hotPressure > coolPressure);
    }

    void raceModeAcceleratesFasterThanRainMode()
    {
        RideCycleSimulator rainSim;
        rainSim.setRidingMode(RidingMode::Rain);
        RideCycleSimulator raceSim;
        raceSim.setRidingMode(RidingMode::Race);

        // Same elapsed time, same target-speed profile (both simulators start at
        // t=0 in the acceleration phase): Race mode's steeper power-delivery
        // curve (smaller time constant) should have the bike further along
        // toward the target speed than Rain mode after the same short interval.
        double rainSpeed = 0.0;
        double raceSpeed = 0.0;
        for (int i = 0; i < 20; ++i) {
            rainSpeed = rainSim.advance(0.05).speedKph;
            raceSpeed = raceSim.advance(0.05).speedKph;
        }
        QVERIFY(raceSpeed > rainSpeed);
    }

    void speedTimeConstantIsSnappierInHigherModes()
    {
        QVERIFY(RideCycleSimulator::speedTimeConstantSecondsForMode(RidingMode::Rain)
                > RideCycleSimulator::speedTimeConstantSecondsForMode(RidingMode::Road));
        QVERIFY(RideCycleSimulator::speedTimeConstantSecondsForMode(RidingMode::Road)
                > RideCycleSimulator::speedTimeConstantSecondsForMode(RidingMode::Sport));
        QVERIFY(RideCycleSimulator::speedTimeConstantSecondsForMode(RidingMode::Sport)
                > RideCycleSimulator::speedTimeConstantSecondsForMode(RidingMode::Race));
    }

    void rainModeLimitsLeanAngleComparedToRace()
    {
        QVERIFY(RideCycleSimulator::leanAngleFactorForMode(RidingMode::Rain)
                < RideCycleSimulator::leanAngleFactorForMode(RidingMode::Race));

        RideCycleSimulator rainSim;
        rainSim.setRidingMode(RidingMode::Rain);
        RideCycleSimulator raceSim;
        raceSim.setRidingMode(RidingMode::Race);
        for (int i = 0; i < 700; ++i) {
            rainSim.advance(0.05);
            raceSim.advance(0.05);
        }
        QVERIFY(std::abs(raceSim.state().leanAngleDeg) >= std::abs(rainSim.state().leanAngleDeg));
    }

    void fuelBurnRateIncreasesWithRpmAndMode()
    {
        const double lowRpmBurn = RideCycleSimulator::fuelBurnRateLitresPerHour(2000.0, RidingMode::Road);
        const double highRpmBurn = RideCycleSimulator::fuelBurnRateLitresPerHour(9000.0, RidingMode::Road);
        QVERIFY(highRpmBurn > lowRpmBurn);

        const double roadBurn = RideCycleSimulator::fuelBurnRateLitresPerHour(6000.0, RidingMode::Road);
        const double raceBurn = RideCycleSimulator::fuelBurnRateLitresPerHour(6000.0, RidingMode::Race);
        QVERIFY(raceBurn > roadBurn);
    }

    void fuelLevelDecreasesOverTimeAndStaysNonNegative()
    {
        RideCycleSimulator sim;
        sim.setRidingMode(RidingMode::Race);
        const double startFuel = sim.state().fuelLitres;
        double lastFuel = startFuel;
        for (int i = 0; i < 200; ++i) {
            const RideState state = sim.advance(0.05);
            QVERIFY(state.fuelLitres <= lastFuel + 1e-9);
            QVERIFY(state.fuelLitres >= 0.0);
            lastFuel = state.fuelLitres;
        }
        QVERIFY(lastFuel < startFuel);
    }

    void ridingModeLabelsAreCapitalised()
    {
        QCOMPARE(QString::fromLatin1(ridingModeLabel(RidingMode::Rain)), QStringLiteral("Rain"));
        QCOMPARE(QString::fromLatin1(ridingModeLabel(RidingMode::Road)), QStringLiteral("Road"));
        QCOMPARE(QString::fromLatin1(ridingModeLabel(RidingMode::Sport)), QStringLiteral("Sport"));
        QCOMPARE(QString::fromLatin1(ridingModeLabel(RidingMode::Race)), QStringLiteral("Race"));
    }
};

QTEST_MAIN(TestRideCycleSimulator)
#include "test_ride_cycle_simulator.moc"
