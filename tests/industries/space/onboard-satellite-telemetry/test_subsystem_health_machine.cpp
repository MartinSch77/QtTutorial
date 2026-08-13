// SPDX-License-Identifier: MIT
#include "SubsystemHealthMachine.h"

#include <QSignalSpy>
#include <QTest>

using namespace qttutorial::space;

class TestSubsystemHealthMachine : public QObject {
    Q_OBJECT
private slots:
    void startsInNominalState()
    {
        SubsystemHealthMachine machine(QStringLiteral("Battery"), 30.0, 100.0, 15.0, 100.0);
        QSignalSpy spy(&machine, &SubsystemHealthMachine::healthChanged);
        machine.start();
        // QStateMachine::start() enters its initial state asynchronously; give the
        // event loop a turn before driving values through transitions that depend
        // on the source state actually being active.
        QTest::qWait(20);
        machine.updateValue(50.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Nominal"));
        QVERIFY(!spy.isEmpty());
    }

    void transitionsToCautionThenCritical()
    {
        SubsystemHealthMachine machine(QStringLiteral("Battery"), 30.0, 100.0, 15.0, 100.0);
        machine.start();
        QTest::qWait(20);
        machine.updateValue(50.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Nominal"));

        machine.updateValue(20.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Caution"));

        machine.updateValue(5.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Critical"));
    }

    void recoversFromCriticalToNominal()
    {
        SubsystemHealthMachine machine(QStringLiteral("Battery"), 30.0, 100.0, 15.0, 100.0);
        machine.start();
        QTest::qWait(20);
        machine.updateValue(5.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Critical"));

        machine.updateValue(80.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Nominal"));
    }
};

QTEST_MAIN(TestSubsystemHealthMachine)
#include "test_subsystem_health_machine.moc"
