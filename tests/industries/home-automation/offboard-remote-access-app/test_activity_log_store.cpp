// SPDX-License-Identifier: MIT
#include "ActivityLogStore.h"

#include <QTest>

using namespace qttutorial::homeautomation::remote;

class TestActivityLogStore : public QObject {
    Q_OBJECT
private slots:
    void opensInMemoryDatabase()
    {
        ActivityLogStore store(QStringLiteral(":memory:"), QStringLiteral("test_opens_in_memory"));
        QVERIFY(store.isOpen());
    }

    void recordsAndQueriesEntriesMostRecentFirst()
    {
        ActivityLogStore store(QStringLiteral(":memory:"), QStringLiteral("test_records_and_queries"));
        QVERIFY(store.record(QStringLiteral("Living Room lights turned on"), 1000));
        QVERIFY(store.record(QStringLiteral("Front Door locked"), 2000));
        QVERIFY(store.record(QStringLiteral("Security system armed"), 3000));

        QCOMPARE(store.entryCount(), 3);
        const std::vector<ActivityLogEntry> recent = store.recentEntries(2);
        QCOMPARE(recent.size(), std::size_t{2});
        QCOMPARE(recent[0].description, QStringLiteral("Security system armed"));
        QCOMPARE(recent[1].description, QStringLiteral("Front Door locked"));
    }

    void recentEntriesRespectsLimit()
    {
        ActivityLogStore store(QStringLiteral(":memory:"), QStringLiteral("test_respects_limit"));
        for (int i = 0; i < 5; ++i) {
            store.record(QStringLiteral("event %1").arg(i), 1000 + i);
        }
        QCOMPARE(store.recentEntries(3).size(), std::size_t{3});
    }
};

QTEST_MAIN(TestActivityLogStore)
#include "test_activity_log_store.moc"
