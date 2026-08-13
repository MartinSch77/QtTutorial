// SPDX-License-Identifier: MIT
#include "OperationHistoryStore.h"

#include <QTest>

using namespace qttutorial::agriculture::ops;

class TestOperationHistoryStore : public QObject {
    Q_OBJECT
private slots:
    void opensInMemoryDatabase()
    {
        OperationHistoryStore store(QStringLiteral(":memory:"), QStringLiteral("test_opens_in_memory"));
        QVERIFY(store.isOpen());
    }

    void recordsAndQueriesSamplesInOrder()
    {
        OperationHistoryStore store(QStringLiteral(":memory:"), QStringLiteral("test_records_and_queries"));
        QVERIFY(store.isOpen());

        QVERIFY(store.recordSample(QStringLiteral("FLD-001"), 1000, 10.0, QStringLiteral("working")));
        QVERIFY(store.recordSample(QStringLiteral("FLD-001"), 2000, 20.0, QStringLiteral("working")));
        QVERIFY(store.recordSample(QStringLiteral("FLD-001"), 3000, 30.0, QStringLiteral("turning")));
        QVERIFY(store.recordSample(QStringLiteral("FLD-002"), 1500, 5.0, QStringLiteral("idle")));

        QCOMPARE(store.sampleCount(QStringLiteral("FLD-001")), 3);

        const std::vector<double> coverage = store.recentCoverage(QStringLiteral("FLD-001"), 10);
        QCOMPARE(coverage.size(), std::size_t{3});
        QCOMPARE(coverage[0], 10.0);
        QCOMPARE(coverage[1], 20.0);
        QCOMPARE(coverage[2], 30.0);
    }

    void limitsRecentCoverageToRequestedCount()
    {
        OperationHistoryStore store(QStringLiteral(":memory:"), QStringLiteral("test_limits_recent_coverage"));
        for (int i = 0; i < 10; ++i) {
            store.recordSample(QStringLiteral("FLD-001"), i * 1000, static_cast<double>(i), QStringLiteral("working"));
        }
        const std::vector<double> coverage = store.recentCoverage(QStringLiteral("FLD-001"), 3);
        QCOMPARE(coverage.size(), std::size_t{3});
        QCOMPARE(coverage[2], 9.0);
    }

    void recordsAndQueriesFuelAndEngineLoadHistory()
    {
        OperationHistoryStore store(QStringLiteral(":memory:"), QStringLiteral("test_records_fuel_and_load"));
        QVERIFY(store.isOpen());

        QVERIFY(store.recordSample(QStringLiteral("FLD-001"), 1000, 10.0, QStringLiteral("working"), 78.0, 95.0));
        QVERIFY(store.recordSample(QStringLiteral("FLD-001"), 2000, 20.0, QStringLiteral("working"), 80.0, 90.0));

        const std::vector<double> fuel = store.recentFuelLevels(QStringLiteral("FLD-001"), 10);
        QCOMPARE(fuel.size(), std::size_t{2});
        QCOMPARE(fuel[0], 95.0);
        QCOMPARE(fuel[1], 90.0);

        const std::vector<double> load = store.recentEngineLoads(QStringLiteral("FLD-001"), 10);
        QCOMPARE(load.size(), std::size_t{2});
        QCOMPARE(load[0], 78.0);
        QCOMPARE(load[1], 80.0);
    }

    void defaultsEngineLoadAndFuelToSentinelWhenNotProvided()
    {
        OperationHistoryStore store(QStringLiteral(":memory:"), QStringLiteral("test_defaults_engine_fuel"));
        QVERIFY(store.recordSample(QStringLiteral("FLD-001"), 1000, 10.0, QStringLiteral("working")));

        const std::vector<double> fuel = store.recentFuelLevels(QStringLiteral("FLD-001"), 10);
        QCOMPARE(fuel.size(), std::size_t{1});
        QCOMPARE(fuel[0], -1.0);
    }
};

QTEST_MAIN(TestOperationHistoryStore)
#include "test_operation_history_store.moc"
