// SPDX-License-Identifier: MIT
#include "SnapshotDiff.h"

#include <QTest>

using namespace qttutorial::homeautomation::remote;

namespace {
Snapshot makeBaseSnapshot()
{
    Snapshot snapshot;
    snapshot.rooms.push_back(RoomState{.name = QStringLiteral("Living Room"), .lightOn = false, .brightness = 50, .blindPosition = 50});
    snapshot.locks.push_back(LockState{.name = QStringLiteral("Front Door"), .locked = true});
    snapshot.armed = false;
    snapshot.thermostatMode = 1;
    snapshot.thermostatTarget = 21.0;
    return snapshot;
}
} // namespace

class TestSnapshotDiff : public QObject {
    Q_OBJECT
private slots:
    void identicalSnapshotsProduceNoChanges()
    {
        const Snapshot snapshot = makeBaseSnapshot();
        QVERIFY(describeChanges(snapshot, snapshot).isEmpty());
    }

    void lightTurningOnIsReported()
    {
        Snapshot before = makeBaseSnapshot();
        Snapshot after = before;
        after.rooms[0].lightOn = true;

        const QStringList changes = describeChanges(before, after);
        QCOMPARE(changes.size(), 1);
        QVERIFY(changes.front().contains(QStringLiteral("Living Room")));
        QVERIFY(changes.front().contains(QStringLiteral("on")));
    }

    void brightnessChangeIsIgnoredWhileLightIsOff()
    {
        Snapshot before = makeBaseSnapshot();
        Snapshot after = before;
        after.rooms[0].brightness = 90;

        QVERIFY(describeChanges(before, after).isEmpty());
    }

    void blindPositionChangeIsAlwaysReported()
    {
        Snapshot before = makeBaseSnapshot();
        Snapshot after = before;
        after.rooms[0].blindPosition = 10;

        const QStringList changes = describeChanges(before, after);
        QCOMPARE(changes.size(), 1);
        QVERIFY(changes.front().contains(QStringLiteral("shades")));
    }

    void lockChangeIsReported()
    {
        Snapshot before = makeBaseSnapshot();
        Snapshot after = before;
        after.locks[0].locked = false;

        const QStringList changes = describeChanges(before, after);
        QCOMPARE(changes.size(), 1);
        QVERIFY(changes.front().contains(QStringLiteral("unlocked")));
    }

    void armedChangeIsReported()
    {
        Snapshot before = makeBaseSnapshot();
        Snapshot after = before;
        after.armed = true;

        const QStringList changes = describeChanges(before, after);
        QCOMPARE(changes.size(), 1);
        QVERIFY(changes.front().contains(QStringLiteral("armed")));
    }

    void thermostatCurrentTemperatureDriftIsNotReported()
    {
        Snapshot before = makeBaseSnapshot();
        Snapshot after = before;
        after.thermostatCurrent = before.thermostatCurrent + 0.5;

        QVERIFY(describeChanges(before, after).isEmpty());
    }

    void sensorTriggerAloneIsNotReported()
    {
        Snapshot before = makeBaseSnapshot();
        before.sensors.push_back(SensorState{.name = QStringLiteral("Front Door"), .triggered = false});
        Snapshot after = before;
        after.sensors[0].triggered = true;

        QVERIFY(describeChanges(before, after).isEmpty());
    }
};

QTEST_MAIN(TestSnapshotDiff)
#include "test_snapshot_diff.moc"
