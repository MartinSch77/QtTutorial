// SPDX-License-Identifier: MIT
#include "LockRegistry.h"

#include <QTest>

using namespace qttutorial::homeautomation;

class TestLockRegistry : public QObject {
    Q_OBJECT
private slots:
    void changingLockStateAppendsLogEntry()
    {
        LockRegistry registry;
        const bool wasLocked = registry.lock(0).locked;
        registry.setLocked(0, !wasLocked, QStringLiteral("Wall Panel"), 1000);

        QCOMPARE(registry.lock(0).locked, !wasLocked);
        QCOMPARE(registry.log().size(), std::size_t{1});
        QCOMPARE(registry.log().front().locked, !wasLocked);
        QCOMPARE(registry.log().front().actor, QStringLiteral("Wall Panel"));
        QCOMPARE(registry.log().front().timestampMs, qint64{1000});
    }

    void settingSameStateDoesNotLog()
    {
        LockRegistry registry;
        const bool current = registry.lock(0).locked;
        registry.setLocked(0, current, QStringLiteral("Wall Panel"), 1000);
        QVERIFY(registry.log().empty());
    }

    void recentLogIsMostRecentFirstAndCapped()
    {
        LockRegistry registry;
        registry.setLocked(0, false, QStringLiteral("A"), 1000);
        registry.setLocked(0, true, QStringLiteral("B"), 2000);
        registry.setLocked(1, false, QStringLiteral("C"), 3000);

        const std::vector<LockLogEntry> recent = registry.recentLog(2);
        QCOMPARE(recent.size(), std::size_t{2});
        QCOMPARE(recent[0].actor, QStringLiteral("C"));
        QCOMPARE(recent[1].actor, QStringLiteral("B"));
    }
};

QTEST_MAIN(TestLockRegistry)
#include "test_lock_registry.moc"
