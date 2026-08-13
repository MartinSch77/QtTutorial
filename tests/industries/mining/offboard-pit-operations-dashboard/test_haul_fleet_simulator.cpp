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

    void payloadStaysWithinPlausibleBounds()
    {
        // Payload may briefly exceed rated capacity during the loading
        // overfill-then-trim window (mirroring the onboard console's
        // HaulCycleSimulator), but never beyond the modelled peak overfill.
        for (int truck = 0; truck < 6; ++truck) {
            for (double t = 0.0; t < 600.0; t += 5.0) {
                const TruckSample sample = HaulFleetSimulator::sampleAt(truck, t);
                QVERIFY(sample.payloadTonnes >= 0.0);
                QVERIFY(sample.payloadTonnes <= HaulFleetSimulator::kPeakLoadingOverfillTonnes);
            }
        }
    }

    void overloadIsFlaggedOnlyDuringTheLoadingOverfillWindow()
    {
        const TruckSample overfilled = HaulFleetSimulator::sampleAt(0, 22.0);
        const TruckSample cruising = HaulFleetSimulator::sampleAt(0, 60.0);
        QVERIFY(overfilled.overloaded);
        QVERIFY(!cruising.overloaded);
    }

    void speedIsLowWhileLoadingAndCruisesWhileHauling()
    {
        QVERIFY(HaulFleetSimulator::speedKphAt(15.0) < HaulFleetSimulator::speedKphAt(60.0));
        QCOMPARE(HaulFleetSimulator::speedKphAt(60.0), 45.0);
    }

    void fuelBurnIncreasesWithPayloadAtFixedSpeed()
    {
        const double emptyFuel = HaulFleetSimulator::fuelLtrPerHourAt(0.0, 45.0);
        const double loadedFuel = HaulFleetSimulator::fuelLtrPerHourAt(HaulFleetSimulator::kRatedCapacityTonnes, 45.0);
        QVERIFY(loadedFuel > emptyFuel);
    }

    void fuelBurnIncreasesWithSpeedAtFixedPayload()
    {
        const double slowFuel = HaulFleetSimulator::fuelLtrPerHourAt(100.0, 10.0);
        const double fastFuel = HaulFleetSimulator::fuelLtrPerHourAt(100.0, 55.0);
        QVERIFY(fastFuel > slowFuel);
    }

    void positionMovesFromShovelToDumpWhileHauling()
    {
        const TruckSample haulingStart = HaulFleetSimulator::sampleAt(0, 31.0);
        const TruckSample haulingLate = HaulFleetSimulator::sampleAt(0, 115.0);
        QCOMPARE(haulingStart.stateLabel, QStringLiteral("Hauling"));
        QCOMPARE(haulingLate.stateLabel, QStringLiteral("Hauling"));
        QVERIFY(haulingLate.positionY > haulingStart.positionY);
    }

    void positionStaysWithinNormalisedPitBounds()
    {
        for (int truck = 0; truck < 6; ++truck) {
            for (double t = 0.0; t < 600.0; t += 5.0) {
                const TruckSample sample = HaulFleetSimulator::sampleAt(truck, t);
                QVERIFY(sample.positionX >= 0.0);
                QVERIFY(sample.positionX <= 1.0);
                QVERIFY(sample.positionY >= 0.0);
                QVERIFY(sample.positionY <= 1.0);
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
