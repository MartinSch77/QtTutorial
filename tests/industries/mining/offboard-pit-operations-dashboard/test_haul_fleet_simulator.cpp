// SPDX-License-Identifier: MIT
#include "HaulFleetSimulator.h"

#include <QTest>

using namespace qttutorial::mining::pit;

class TestHaulFleetSimulator : public QObject {
    Q_OBJECT
private slots:
    void producesDistinctIdsAndLocations()
    {
        const TruckSample a = HaulFleetSimulator::sampleAt(0, 0.0);
        const TruckSample b = HaulFleetSimulator::sampleAt(1, 0.0);
        QVERIFY(a.id != b.id);
        QCOMPARE(a.id, QStringLiteral("MT-001"));
    }

    void trucksArePhaseShiftedAcrossTheHaulCycle()
    {
        const TruckSample truck0 = HaulFleetSimulator::sampleAt(0, 0.0);
        const TruckSample truck1 = HaulFleetSimulator::sampleAt(1, 0.0);
        QCOMPARE(truck0.stateLabel, QStringLiteral("Loading"));
        QCOMPARE(truck1.stateLabel, QStringLiteral("Hauling"));
    }

    void payloadStaysWithinRatedBounds()
    {
        for (int truck = 0; truck < 6; ++truck) {
            for (double t = 0.0; t < 600.0; t += 5.0) {
                const TruckSample sample = HaulFleetSimulator::sampleAt(truck, t);
                QVERIFY(sample.payloadTonnes >= 0.0);
                QVERIFY(sample.payloadTonnes <= HaulFleetSimulator::kRatedCapacityTonnes);
            }
        }
    }

    void cumulativeTonnesIsZeroBeforeFirstCycleCompletes()
    {
        const TruckSample sample = HaulFleetSimulator::sampleAt(0, 100.0);
        QCOMPARE(sample.cumulativeTonnesHauled, 0.0);
    }

    void cumulativeTonnesIncreasesByRatedCapacityPerCompletedCycle()
    {
        const TruckSample afterOneCycle = HaulFleetSimulator::sampleAt(0, HaulFleetSimulator::kCyclePeriodSeconds);
        const TruckSample afterTwoCycles =
            HaulFleetSimulator::sampleAt(0, HaulFleetSimulator::kCyclePeriodSeconds * 2.0);
        QCOMPARE(afterOneCycle.cumulativeTonnesHauled, HaulFleetSimulator::kRatedCapacityTonnes);
        QCOMPARE(afterTwoCycles.cumulativeTonnesHauled, HaulFleetSimulator::kRatedCapacityTonnes * 2.0);
    }
};

QTEST_MAIN(TestHaulFleetSimulator)
#include "test_haul_fleet_simulator.moc"
