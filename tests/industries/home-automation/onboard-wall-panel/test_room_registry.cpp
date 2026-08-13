// SPDX-License-Identifier: MIT
#include "RoomRegistry.h"

#include <QTest>

using namespace qttutorial::homeautomation;

class TestRoomRegistry : public QObject {
    Q_OBJECT
private slots:
    void defaultRoomsAreNonEmpty()
    {
        RoomRegistry registry;
        QVERIFY(registry.count() > 0);
    }

    void setLightOnTogglesState()
    {
        RoomRegistry registry;
        registry.setLightOn(0, true);
        QVERIFY(registry.room(0).lightOn);
        registry.setLightOn(0, false);
        QVERIFY(!registry.room(0).lightOn);
    }

    void setBrightnessClampsToValidRange()
    {
        RoomRegistry registry;
        registry.setBrightness(0, 150);
        QCOMPARE(registry.room(0).brightness, 100);
        registry.setBrightness(0, -20);
        QCOMPARE(registry.room(0).brightness, 0);
        registry.setBrightness(0, 42);
        QCOMPARE(registry.room(0).brightness, 42);
    }

    void setBlindPositionClampsToValidRange()
    {
        RoomRegistry registry;
        registry.setBlindPosition(0, 200);
        QCOMPARE(registry.room(0).blindPosition, 100);
        registry.setBlindPosition(0, -5);
        QCOMPARE(registry.room(0).blindPosition, 0);
    }

    void outOfRangeIndexIsIgnoredSafely()
    {
        RoomRegistry registry;
        const int before = registry.count();
        registry.setLightOn(999, true);
        registry.setBrightness(-1, 50);
        QCOMPARE(registry.count(), before);
    }
};

QTEST_MAIN(TestRoomRegistry)
#include "test_room_registry.moc"
