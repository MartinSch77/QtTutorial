// SPDX-License-Identifier: MIT
#include "AlarmStateMachine.h"

#include <QTest>

using namespace qttutorial::medical;

class TestAlarmStateMachine : public QObject {
    Q_OBJECT
private slots:
    void staysNormalForHealthyVitals()
    {
        AlarmStateMachine machine;
        QCOMPARE(machine.update(72.0, 97.0), AlarmLevel::Normal);
    }

    void entersWarningThenCritical()
    {
        AlarmStateMachine machine;
        QCOMPARE(machine.update(112.0, 97.0), AlarmLevel::Warning);
        QCOMPARE(machine.update(128.0, 97.0), AlarmLevel::Critical);
    }

    void doesNotFlickerOnBorderlineWarningValues()
    {
        AlarmStateMachine machine;
        QCOMPARE(machine.update(112.0, 97.0), AlarmLevel::Warning);
        // 105 is below the enter threshold (110) but above the exit threshold
        // (100): hysteresis must keep the alarm in Warning, not bounce back
        // to Normal.
        for (int i = 0; i < 5; ++i) {
            QCOMPARE(machine.update(105.0, 97.0), AlarmLevel::Warning);
        }
    }

    void exitsWarningOnlyOnceComfortablyBelowThreshold()
    {
        AlarmStateMachine machine;
        machine.update(112.0, 97.0);
        QCOMPARE(machine.level(), AlarmLevel::Warning);
        QCOMPARE(machine.update(99.0, 97.0), AlarmLevel::Normal);
    }

    void criticalDropsToWarningNotDirectlyToNormalWhenStillElevated()
    {
        AlarmStateMachine machine;
        machine.update(130.0, 97.0);
        QCOMPARE(machine.level(), AlarmLevel::Critical);
        QCOMPARE(machine.update(112.0, 97.0), AlarmLevel::Warning);
    }

    void lowSpo2AloneTriggersCritical()
    {
        AlarmStateMachine machine;
        QCOMPARE(machine.update(75.0, 85.0), AlarmLevel::Critical);
    }
};

QTEST_MAIN(TestAlarmStateMachine)
#include "test_alarm_state_machine.moc"
