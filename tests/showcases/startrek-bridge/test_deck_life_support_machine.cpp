// SPDX-License-Identifier: MIT
#include "DeckLifeSupportMachine.h"

#include <QSignalSpy>
#include <QTest>

using namespace qttutorial::startrek_bridge;

class TestDeckLifeSupportMachine : public QObject {
    Q_OBJECT
private slots:
    void startsInNominalState()
    {
        DeckLifeSupportMachine machine(QStringLiteral("Bridge"), 55.0, 30.0);
        QSignalSpy spy(&machine, &DeckLifeSupportMachine::statusChanged);
        machine.start();
        // QStateMachine::start() enters its initial state asynchronously; give the
        // event loop a turn before driving values through transitions that depend
        // on the source state actually being active.
        QTest::qWait(20);
        machine.updateValue(90.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Nominal"));
        QVERIFY(!spy.isEmpty());
    }

    void transitionsToCautionThenCritical()
    {
        DeckLifeSupportMachine machine(QStringLiteral("Cargo Bay"), 55.0, 30.0);
        machine.start();
        QTest::qWait(20);
        machine.updateValue(90.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Nominal"));

        machine.updateValue(40.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Caution"));

        machine.updateValue(10.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Critical"));
    }

    void recoversFromCriticalToNominal()
    {
        DeckLifeSupportMachine machine(QStringLiteral("Shuttle Bay"), 55.0, 30.0);
        machine.start();
        QTest::qWait(20);
        machine.updateValue(10.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Critical"));

        machine.updateValue(85.0);
        QTRY_COMPARE(machine.currentStateName(), QStringLiteral("Nominal"));
    }
};

QTEST_MAIN(TestDeckLifeSupportMachine)
#include "test_deck_life_support_machine.moc"
