// SPDX-License-Identifier: GPL-3.0-or-later
#include "AlarmAudioController.h"

#include <QSignalSpy>
#include <QTest>

using qttutorial::offboard_digital_twin::AlarmAudioController;

class TestAlarmAudioController : public QObject {
    Q_OBJECT
private slots:
    void computeCueIsFullGainAndCenteredWhenColocated()
    {
        const auto cue = AlarmAudioController::computeCue(QVector3D(0, 0, 0), QVector3D(0, 0, -1), QVector3D(0, 0, 0));
        QCOMPARE(cue.gain, 1.0);
        QCOMPARE(cue.pan, 0.0);
    }

    void computeCuePansRightWhenSourceIsToTheRight()
    {
        // Listener facing -Z (world forward), source to its +X (right).
        const auto cue = AlarmAudioController::computeCue(QVector3D(0, 0, 0), QVector3D(0, 0, -1), QVector3D(5, 0, 0));
        QVERIFY(cue.pan > 0.5);
    }

    void computeCuePansLeftWhenSourceIsToTheLeft()
    {
        const auto cue = AlarmAudioController::computeCue(QVector3D(0, 0, 0), QVector3D(0, 0, -1), QVector3D(-5, 0, 0));
        QVERIFY(cue.pan < -0.5);
    }

    void computeCueGainFallsOffWithDistanceAndFloorsAtZero()
    {
        const auto near = AlarmAudioController::computeCue(QVector3D(0, 0, 0), QVector3D(0, 0, -1), QVector3D(0, 0, -2));
        const auto far = AlarmAudioController::computeCue(QVector3D(0, 0, 0), QVector3D(0, 0, -1), QVector3D(0, 0, -50));
        QVERIFY(near.gain > far.gain);
        QCOMPARE(far.gain, 0.0);
    }

    void startAlarmSetsActiveAndComputesCue()
    {
        AlarmAudioController controller;
        QSignalSpy activeSpy(&controller, &AlarmAudioController::activeChanged);
        controller.updateListener(QVector3D(0, 0, 0), QVector3D(0, 0, -1));
        controller.startAlarm(QVector3D(3, 0, -3));

        QVERIFY(controller.isActive());
        QCOMPARE(activeSpy.count(), 1);
        QVERIFY(controller.gain() > 0.0);
    }

    void stopAlarmClearsActive()
    {
        AlarmAudioController controller;
        controller.startAlarm(QVector3D(1, 0, 1));
        QVERIFY(controller.isActive());

        controller.stopAlarm();
        QVERIFY(!controller.isActive());
    }

    void realAudioAvailableReflectsBuildConfiguration()
    {
        // Whichever way it goes on this build, the property must be
        // self-consistent with the static helper.
        QCOMPARE(AlarmAudioController::realAudioAvailable(), AlarmAudioController().property("realAudioAvailable").toBool());
    }
};

QTEST_MAIN(TestAlarmAudioController)
#include "test_alarm_audio_controller.moc"
