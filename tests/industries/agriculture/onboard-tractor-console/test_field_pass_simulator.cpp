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
        QVERIFY(FieldPassSimulator::kFuelPercentPerKmWorking > FieldPassSimulator::kFuelPercentPerKmIdle);
    }

    void workingDepthIsZeroWhenNotEngaged()
    {
        QCOMPARE(FieldPassSimulator::workingDepthAt(false), 0.0);
        QVERIFY(FieldPassSimulator::workingDepthAt(true) > 0.0);
    }
};

QTEST_MAIN(TestFieldPassSimulator)
#include "test_field_pass_simulator.moc"
