// SPDX-License-Identifier: MIT
#include "SubsystemHealthMachine.h"

#include <QSignalSpy>
#include <QTest>

using namespace qttutorial::defence;

class TestSubsystemHealthMachine : public QObject {
    Q_OBJECT
private slots:
    void startsInNominalState()
    {
        SubsystemHealthMachine machine(QStringLiteral("Propulsion"), 30.0, 100.0, 15.0, 100.0);
        QSignalSpy spy(&machine, &SubsystemHealthMachine::healthChanged);
        machine.start();
        QTest::qWait(20);
        machine.updateValue(70.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Nominal"));
        QVERIFY(!spy.isEmpty());
    }

    void transitionsToCautionThenCritical()
    {
        SubsystemHealthMachine machine(QStringLiteral("Power"), 40.0, 100.0, 20.0, 100.0);
        machine.start();
        QTest::qWait(20);
        machine.updateValue(80.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Nominal"));

        machine.updateValue(30.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Caution"));

        machine.updateValue(10.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Critical"));
    }

    void recoversFromCriticalToNominal()
    {
        SubsystemHealthMachine machine(QStringLiteral("Comms"), 50.0, 100.0, 25.0, 100.0);
        machine.start();
        QTest::qWait(20);
        machine.updateValue(10.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Critical"));

        machine.updateValue(90.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Nominal"));
    }
};

QTEST_MAIN(TestSubsystemHealthMachine)
#include "test_defence_subsystem_health_machine.moc"
