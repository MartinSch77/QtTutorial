// SPDX-License-Identifier: MIT
#include "FieldPassSimulator.h"

#include <QTest>

#include <cmath>

using namespace qttutorial::agriculture;

class TestFieldPassSimulator : public QObject {
    Q_OBJECT
private slots:
    void coverageProgressesMonotonicallyThenWraps()
    {
        FieldPassSimulator sim;
        double lastCoverage = -1.0;
        int lastPassNumber = sim.state().passNumber;
        bool sawWrap = false;
        for (int i = 0; i < 4000; ++i) {
            const FieldPassState state = sim.advance(0.1);
            if (state.coveragePercent < lastCoverage) {
                sawWrap = true;
                QCOMPARE(state.passNumber, lastPassNumber + 1);
            } else {
                QVERIFY(state.coveragePercent >= lastCoverage - 1e-9);
            }
            lastCoverage = state.coveragePercent;
            lastPassNumber = state.passNumber;
        }
        QVERIFY(sawWrap);
    }

    void implementDisengagesInTurnZonesAtRowEnds()
    {
        QVERIFY(!FieldPassSimulator::isImplementEngagedAt(0.0));
        QVERIFY(!FieldPassSimulator::isImplementEngagedAt(0.99));
        QVERIFY(FieldPassSimulator::isImplementEngagedAt(0.5));
    }

    void crossTrackErrorIsDeterministicAndBounded()
    {
        for (double distance = 0.0; distance < 2000.0; distance += 3.7) {
            const double first = FieldPassSimulator::crossTrackErrorAt(distance);
            const double second = FieldPassSimulator::crossTrackErrorAt(distance);
            QCOMPARE(first, second);
            QVERIFY(std::abs(first) <= 9.01);
        }
    }

    void engineLoadIsHigherWhenEngaged()
    {
        const double disengagedLoad = FieldPassSimulator::engineLoadAt(false, 2.0);
        const double engagedLoad = FieldPassSimulator::engineLoadAt(true, 2.0);
        QVERIFY(engagedLoad > disengagedLoad);
    }

    void fuelBurnsFasterWhileWorkingThanTurning()
    {
        const double idleLoad = FieldPassSimulator::engineLoadAt(false, 2.0);
        const double workingLoad = FieldPassSimulator::engineLoadAt(true, 2.0);
        QVERIFY(FieldPassSimulator::fuelPercentPerKmAt(workingLoad) > FieldPassSimulator::fuelPercentPerKmAt(idleLoad));
    }

    void fuelBurnRateIncreasesMonotonicallyWithEngineLoad()
    {
        double lastRate = -1.0;
        for (double load = 0.0; load <= 100.0; load += 5.0) {
            const double rate = FieldPassSimulator::fuelPercentPerKmAt(load);
            QVERIFY(rate > lastRate);
            lastRate = rate;
        }
    }

    void workingDepthIsZeroWhenNotEngaged()
    {
        QCOMPARE(FieldPassSimulator::workingDepthAt(false), 0.0);
        QVERIFY(FieldPassSimulator::workingDepthAt(true) > 0.0);
    }

    void yieldRateIsZeroWhenDisengagedAndScalesWithSpeedWhenEngaged()
    {
        QCOMPARE(FieldPassSimulator::yieldRateAt(false, FieldPassSimulator::kWorkingSpeedKph), 0.0);

        const double atWorkingSpeed = FieldPassSimulator::yieldRateAt(true, FieldPassSimulator::kWorkingSpeedKph);
        QVERIFY(atWorkingSpeed > 0.0);

        const double atHalfSpeed = FieldPassSimulator::yieldRateAt(true, FieldPassSimulator::kWorkingSpeedKph / 2.0);
        QVERIFY(atHalfSpeed < atWorkingSpeed);
        QCOMPARE(atHalfSpeed, atWorkingSpeed / 2.0);
    }

    void rowIndexAlternatesDirectionAndWrapsAcrossTheField()
    {
        QCOMPARE(FieldPassSimulator::rowIndexForPass(1), 0);
        QVERIFY(FieldPassSimulator::isMovingForward(0));

        QCOMPARE(FieldPassSimulator::rowIndexForPass(2), 1);
        QVERIFY(!FieldPassSimulator::isMovingForward(1));

        const int wrappedRow = FieldPassSimulator::rowIndexForPass(FieldPassSimulator::kFieldRowCount + 1);
        QCOMPARE(wrappedRow, 0);
    }

    void simulatorReportsRequestedImplementKind()
    {
        FieldPassSimulator plow(ImplementKind::Plow);
        QCOMPARE(plow.state().implementKind, ImplementKind::Plow);

        FieldPassSimulator defaultImplement;
        QCOMPARE(defaultImplement.state().implementKind, ImplementKind::Planter);
    }

    void rowIndexAdvancesAsPassesAccumulate()
    {
        FieldPassSimulator sim;
        int lastRowIndex = sim.state().rowIndex;
        bool sawRowChange = false;
        for (int i = 0; i < 4000; ++i) {
            const FieldPassState state = sim.advance(0.1);
            if (state.rowIndex != lastRowIndex) {
                sawRowChange = true;
                QCOMPARE(state.movingForward, FieldPassSimulator::isMovingForward(state.rowIndex));
            }
            lastRowIndex = state.rowIndex;
        }
        QVERIFY(sawRowChange);
    }
};

QTEST_MAIN(TestFieldPassSimulator)
#include "test_field_pass_simulator.moc"
