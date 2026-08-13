// SPDX-License-Identifier: MIT
#include "LockRegistry.h"
#include "RoomRegistry.h"
#include "SceneRegistry.h"
#include "SecurityCenter.h"
#include "ThermostatModel.h"

#include <QTest>

using namespace qttutorial::homeautomation;

class TestSceneRegistry : public QObject {
    Q_OBJECT
private slots:
    void allFourScenesAreDefined()
    {
        QCOMPARE(SceneRegistry::scenes().size(), std::size_t{4});
    }

    void awaySceneTurnsOffAllLightsLocksDoorsAndArmsSecurity()
    {
        RoomRegistry rooms;
        LockRegistry locks;
        SecurityCenter security;
        // Start from a state where the scene's effect is observable.
        for (int i = 0; i < rooms.count(); ++i) {
            rooms.setLightOn(i, true);
        }
        for (int i = 0; i < locks.count(); ++i) {
            locks.setLocked(i, false, QStringLiteral("test"), 0);
        }
        security.setArmed(false);

        SceneRegistry::applyToRooms(Scene::Away, rooms);
        SceneRegistry::applyToLocks(Scene::Away, locks, QStringLiteral("Scene: Away"), 1000);
        SceneRegistry::applyToSecurity(Scene::Away, security);

        for (int i = 0; i < rooms.count(); ++i) {
            QVERIFY(!rooms.room(i).lightOn);
        }
        for (int i = 0; i < locks.count(); ++i) {
            QVERIFY(locks.lock(i).locked);
        }
        QVERIFY(security.armed());
    }

    void awaySceneSetsThermostatBackForEnergySaving()
    {
        ThermostatModel thermostat;
        thermostat.setTargetTemperature(21.0);
        SceneRegistry::applyToThermostat(Scene::Away, thermostat);
        QCOMPARE(thermostat.targetTemperature(), SceneRegistry::definition(Scene::Away).thermostatTarget);
        QVERIFY(thermostat.targetTemperature() < 21.0);
    }

    void homeSceneTurnsLightsBackOnAndDisarms()
    {
        RoomRegistry rooms;
        LockRegistry locks;
        SecurityCenter security;
        for (int i = 0; i < rooms.count(); ++i) {
            rooms.setLightOn(i, false);
        }
        security.setArmed(true);

        SceneRegistry::applyToRooms(Scene::Home, rooms);
        SceneRegistry::applyToSecurity(Scene::Home, security);

        for (int i = 0; i < rooms.count(); ++i) {
            QVERIFY(rooms.room(i).lightOn);
        }
        QVERIFY(!security.armed());
    }

    void nightSceneDimsLightsRatherThanTurningThemOff()
    {
        RoomRegistry rooms;
        SceneRegistry::applyToRooms(Scene::Night, rooms);
        for (int i = 0; i < rooms.count(); ++i) {
            QVERIFY(rooms.room(i).lightOn);
            QVERIFY(rooms.room(i).brightness < SceneRegistry::definition(Scene::Home).brightness);
        }
    }

    void applyingLocksSceneTwiceInARowIsIdempotentAndLogsOnlyOnce()
    {
        LockRegistry locks;
        for (int i = 0; i < locks.count(); ++i) {
            locks.setLocked(i, false, QStringLiteral("test"), 0);
        }

        SceneRegistry::applyToLocks(Scene::Away, locks, QStringLiteral("Scene: Away"), 1000);
        const std::size_t afterFirst = locks.log().size();
        SceneRegistry::applyToLocks(Scene::Away, locks, QStringLiteral("Scene: Away"), 2000);
        QCOMPARE(locks.log().size(), afterFirst);
    }

    void sceneThatDoesNotTouchLocksLeavesThemUnchanged()
    {
        LockRegistry locks;
        locks.setLocked(0, false, QStringLiteral("test"), 0);
        SceneRegistry::applyToLocks(Scene::Home, locks, QStringLiteral("Scene: Home"), 1000);
        QVERIFY(!locks.lock(0).locked);
    }
};

QTEST_MAIN(TestSceneRegistry)
#include "test_scene_registry.moc"
