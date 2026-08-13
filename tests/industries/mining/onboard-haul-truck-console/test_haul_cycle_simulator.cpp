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
};

QTEST_MAIN(TestHaulCycleSimulator)
#include "test_haul_cycle_simulator.moc"
