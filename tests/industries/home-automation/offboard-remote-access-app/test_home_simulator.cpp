// SPDX-License-Identifier: MIT
#include "HomeSimulator.h"

#include <QTest>

using namespace qttutorial::homeautomation::remote;

class TestHomeSimulator : public QObject {
    Q_OBJECT
private slots:
    void setLightTogglesNamedRoom()
    {
        HomeSimulator simulator;
        const QString room = simulator.snapshot().rooms.front().name;
        const bool before = simulator.snapshot().rooms.front().lightOn;

        QVERIFY(simulator.applyCommand(Command{.type = QStringLiteral("setLight"), .target = room, .value = before ? 0.0 : 1.0}));
        QCOMPARE(simulator.snapshot().rooms.front().lightOn, !before);
    }

    void settingLightToSameStateIsANoOp()
    {
        HomeSimulator simulator;
        const QString room = simulator.snapshot().rooms.front().name;
        const bool current = simulator.snapshot().rooms.front().lightOn;

        QVERIFY(!simulator.applyCommand(Command{.type = QStringLiteral("setLight"), .target = room, .value = current ? 1.0 : 0.0}));
    }

    void setBrightnessClampsAboveAndBelowRange()
    {
        HomeSimulator simulator;
        const QString room = simulator.snapshot().rooms.front().name;

        simulator.applyCommand(Command{.type = QStringLiteral("setBrightness"), .target = room, .value = 250.0});
        QCOMPARE(simulator.snapshot().rooms.front().brightness, 100);

        simulator.applyCommand(Command{.type = QStringLiteral("setBrightness"), .target = room, .value = -50.0});
        QCOMPARE(simulator.snapshot().rooms.front().brightness, 0);
    }

    void unknownRoomTargetIsIgnored()
    {
        HomeSimulator simulator;
        QVERIFY(!simulator.applyCommand(Command{.type = QStringLiteral("setLight"), .target = QStringLiteral("Nonexistent"), .value = 1.0}));
    }

    void unknownCommandTypeIsIgnored()
    {
        HomeSimulator simulator;
        QVERIFY(!simulator.applyCommand(Command{.type = QStringLiteral("doSomethingUnsupported")}));
    }

    void setLockTogglesNamedLock()
    {
        HomeSimulator simulator;
        const QString lockName = simulator.snapshot().locks.front().name;
        const bool before = simulator.snapshot().locks.front().locked;

        QVERIFY(simulator.applyCommand(Command{.type = QStringLiteral("setLock"), .target = lockName, .value = before ? 0.0 : 1.0}));
        QCOMPARE(simulator.snapshot().locks.front().locked, !before);
    }

    void setArmedTogglesSecuritySystem()
    {
        HomeSimulator simulator;
        const bool before = simulator.snapshot().armed;
        QVERIFY(simulator.applyCommand(Command{.type = QStringLiteral("setArmed"), .value = before ? 0.0 : 1.0}));
        QCOMPARE(simulator.snapshot().armed, !before);
    }

    void setThermostatModeAndTargetUpdateSnapshot()
    {
        HomeSimulator simulator;
        QVERIFY(simulator.applyCommand(Command{.type = QStringLiteral("setThermostatMode"), .value = 2.0}));
        QCOMPARE(simulator.snapshot().thermostatMode, 2);

        QVERIFY(simulator.applyCommand(Command{.type = QStringLiteral("setThermostatTarget"), .value = 23.5}));
        QCOMPARE(simulator.snapshot().thermostatTarget, 23.5);
    }
};

QTEST_MAIN(TestHomeSimulator)
#include "test_home_simulator.moc"
