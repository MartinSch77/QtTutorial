// SPDX-License-Identifier: MIT
#include "HaulCycleSimulator.h"

#include <QTest>

using namespace qttutorial::mining;

class TestHaulCycleSimulator : public QObject {
    Q_OBJECT
private slots:
    void cyclesThroughAllFourStatesInOrder()
    {
        HaulCycleSimulator sim;
        std::vector<HaulState> observedOrder;
        HaulState lastState = sim.state().haulState;
        observedOrder.push_back(lastState);

        const int steps = static_cast<int>(HaulCycleSimulator::kCyclePeriodSeconds) * 2;
        for (int i = 0; i < steps; ++i) {
            const HaulTruckState state = sim.advance(1.0);
            if (state.haulState != lastState) {
                observedOrder.push_back(state.haulState);
                lastState = state.haulState;
            }
        }

        QVERIFY(observedOrder.size() >= 8);
        for (std::size_t i = 0; i < observedOrder.size(); ++i) {
            const HaulState expected = static_cast<HaulState>(i % 4);
            QCOMPARE(observedOrder[i], expected);
        }
    }

    void payloadIsZeroWhileReturning()
    {
        QCOMPARE(HaulCycleSimulator::payloadAt(150.0), 0.0);
        QCOMPARE(HaulCycleSimulator::payloadAt(190.0), 0.0);
    }

    void payloadIsAtRatedCapacityWhileHauling()
    {
        QCOMPARE(HaulCycleSimulator::payloadAt(80.0), HaulCycleSimulator::kRatedCapacityTonnes);
        QCOMPARE(HaulCycleSimulator::payloadAt(115.0), HaulCycleSimulator::kRatedCapacityTonnes);
    }

    void payloadRampsUpDuringLoading()
    {
        QVERIFY(HaulCycleSimulator::payloadAt(5.0) < HaulCycleSimulator::payloadAt(15.0));
        QCOMPARE(HaulCycleSimulator::payloadAt(30.0), HaulCycleSimulator::kRatedCapacityTonnes);
    }

    void overloadDetectionAtCorrectThreshold()
    {
        QVERIFY(!HaulCycleSimulator::isOverloaded(HaulCycleSimulator::kRatedCapacityTonnes));
        QVERIFY(!HaulCycleSimulator::isOverloaded(299.0));
        QVERIFY(HaulCycleSimulator::isOverloaded(300.1));
        QVERIFY(HaulCycleSimulator::isOverloaded(315.0));
    }

    void engineTempRisesWhileLoadingAndHauling()
    {
        QVERIFY(HaulCycleSimulator::engineTempAt(5.0) < HaulCycleSimulator::engineTempAt(29.0));
        QVERIFY(HaulCycleSimulator::engineTempAt(35.0) < HaulCycleSimulator::engineTempAt(115.0));
    }

    void engineTempEasesWhileReturning()
    {
        QVERIFY(HaulCycleSimulator::engineTempAt(140.0) > HaulCycleSimulator::engineTempAt(190.0));
    }

    void retarderTempStaysLowUntilDescent()
    {
        QCOMPARE(HaulCycleSimulator::retarderTempAt(50.0), 40.0);
        QCOMPARE(HaulCycleSimulator::retarderTempAt(90.0), 40.0);
    }

    void retarderTempRisesDuringDescentAndDumping()
    {
        QVERIFY(HaulCycleSimulator::retarderTempAt(105.0) > HaulCycleSimulator::retarderTempAt(90.0));
        QVERIFY(HaulCycleSimulator::retarderTempAt(119.0) > HaulCycleSimulator::retarderTempAt(105.0));
    }

    void retarderTempCoolsWhileReturning()
    {
        QVERIFY(HaulCycleSimulator::retarderTempAt(190.0) < HaulCycleSimulator::retarderTempAt(140.0));
    }

    void tyreTempRisesWithPayload()
    {
        // Heavier payload should drive a faster, higher tyre-temperature rise
        // for the same wheel: the whole point of correlating tyre heat with
        // load rather than varying it independently.
        const double coolTemp = HaulCycleSimulator::tyreTempAt(0, 0.0);
        const double hotTemp = HaulCycleSimulator::tyreTempAt(0, HaulCycleSimulator::kRatedCapacityTonnes);
        QVERIFY(hotTemp > coolTemp);
    }

    void rearTyresRunHotterThanFrontAtTheSamePayload()
    {
        const double frontTemp = HaulCycleSimulator::tyreTempAt(0, HaulCycleSimulator::kRatedCapacityTonnes);
        const double rearTemp = HaulCycleSimulator::tyreTempAt(2, HaulCycleSimulator::kRatedCapacityTonnes);
        QVERIFY(rearTemp > frontTemp);
    }

    void speedIsLowWhileLoadingAndCruisesWhileHauling()
    {
        QVERIFY(HaulCycleSimulator::speedKphAt(15.0) < HaulCycleSimulator::speedKphAt(60.0));
        QCOMPARE(HaulCycleSimulator::speedKphAt(60.0), 45.0);
    }

    void speedDropsDescendingIntoTheDumpPointAndIsZeroWhileDumping()
    {
        QVERIFY(HaulCycleSimulator::speedKphAt(115.0) < HaulCycleSimulator::speedKphAt(60.0));
        QCOMPARE(HaulCycleSimulator::speedKphAt(130.0), 0.0);
    }

    void speedRampsUpEmptyWhileReturning()
    {
        QVERIFY(HaulCycleSimulator::speedKphAt(140.0) < HaulCycleSimulator::speedKphAt(145.0));
    }

    void fuelBurnIncreasesWithPayloadAtFixedSpeed()
    {
        const double emptyFuel = HaulCycleSimulator::fuelLtrPerHourAt(0.0, 45.0);
        const double loadedFuel = HaulCycleSimulator::fuelLtrPerHourAt(HaulCycleSimulator::kRatedCapacityTonnes, 45.0);
        QVERIFY(loadedFuel > emptyFuel);
    }

    void fuelBurnIncreasesWithSpeedAtFixedPayload()
    {
        const double slowFuel = HaulCycleSimulator::fuelLtrPerHourAt(100.0, 10.0);
        const double fastFuel = HaulCycleSimulator::fuelLtrPerHourAt(100.0, 55.0);
        QVERIFY(fastFuel > slowFuel);
    }

    void expectedSpeedRangeIsNarrowestWhileLoadingAndWidestWhileHauling()
    {
        const SpeedRangeKph loadingRange = HaulCycleSimulator::expectedSpeedRangeAt(HaulState::Loading);
        const SpeedRangeKph haulingRange = HaulCycleSimulator::expectedSpeedRangeAt(HaulState::Hauling);
        QVERIFY(loadingRange.maxKph < haulingRange.maxKph);
        QVERIFY(haulingRange.minKph > 0.0);
    }

    void advanceKeepsSpeedFuelAndTyreTempsWithinSaneBounds()
    {
        HaulCycleSimulator sim;
        for (int i = 0; i < static_cast<int>(HaulCycleSimulator::kCyclePeriodSeconds); ++i) {
            const HaulTruckState state = sim.advance(1.0);
            QVERIFY(state.speedKph >= 0.0);
            QVERIFY(state.speedKph <= 70.0);
            QVERIFY(state.fuelLtrPerHour > 0.0);
            for (const double temp : state.tyreTempsC) {
                QVERIFY(temp >= 50.0);
                QVERIFY(temp <= 100.0);
            }
        }
    }
};

QTEST_MAIN(TestHaulCycleSimulator)
#include "test_haul_cycle_simulator.moc"
