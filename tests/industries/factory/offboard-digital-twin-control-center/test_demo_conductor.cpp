// SPDX-License-Identifier: GPL-3.0-or-later
#include "DemoConductor.h"

#include <QSignalSpy>
#include <QTest>

using qttutorial::offboard_digital_twin::DemoConductor;

class TestDemoConductor : public QObject {
Q_OBJECT
private slots:
    void startsIdleAndNotPlaying()
    {
        DemoConductor conductor;
        QVERIFY(!conductor.isPlaying());
        QCOMPARE(conductor.stepIndex(), -1);
    }

    void playEntersFirstBeatAndEmitsExplore()
    {
        DemoConductor conductor;
        QSignalSpy exploreSpy(&conductor, &DemoConductor::requestExplore);
        conductor.play();

        QVERIFY(conductor.isPlaying());
        QTRY_COMPARE(conductor.stepIndex(), 0);
        QCOMPARE(exploreSpy.count(), 1);
        QVERIFY(!conductor.narration().isEmpty());
    }

    void stopReturnsToIdle()
    {
        DemoConductor conductor;
        conductor.play();
        QTRY_COMPARE(conductor.stepIndex(), 0);

        conductor.stop();
        QVERIFY(!conductor.isPlaying());
        QCOMPARE(conductor.stepIndex(), -1);
    }

    void playAgainAfterStopRestartsFromFirstBeat()
    {
        DemoConductor conductor;
        conductor.play();
        QTRY_COMPARE(conductor.stepIndex(), 0);
        conductor.stop();

        QSignalSpy exploreSpy(&conductor, &DemoConductor::requestExplore);
        conductor.play();
        QTRY_COMPARE(conductor.stepIndex(), 0);
        QCOMPARE(exploreSpy.count(), 1);
    }
};

QTEST_MAIN(TestDemoConductor)
#include "test_demo_conductor.moc"
