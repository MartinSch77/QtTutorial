// SPDX-License-Identifier: MIT
#include "HaulHistoryStore.h"

#include <QTest>

using namespace qttutorial::mining::pit;

class TestHaulHistoryStore : public QObject {
    Q_OBJECT
private slots:
    void opensInMemoryDatabase()
    {
        HaulHistoryStore store(QStringLiteral(":memory:"), QStringLiteral("test_mining_opens_in_memory"));
        QVERIFY(store.isOpen());
    }

    void recordsAndQueriesSamplesInOrder()
    {
        HaulHistoryStore store(QStringLiteral(":memory:"), QStringLiteral("test_mining_records_and_queries"));
        QVERIFY(store.isOpen());

        QVERIFY(store.recordSample(QStringLiteral("MT-001"), 1000, QStringLiteral("Loading"), 100.0, 0.0));
        QVERIFY(store.recordSample(QStringLiteral("MT-001"), 2000, QStringLiteral("Hauling"), 300.0, 0.0));
        QVERIFY(store.recordSample(QStringLiteral("MT-001"), 3000, QStringLiteral("Returning"), 0.0, 300.0));
        QVERIFY(store.recordSample(QStringLiteral("MT-002"), 1500, QStringLiteral("Loading"), 50.0, 0.0));

        QCOMPARE(store.sampleCount(QStringLiteral("MT-001")), 3);

        const std::vector<double> tonnes = store.recentCumulativeTonnes(QStringLiteral("MT-001"), 10);
        QCOMPARE(tonnes.size(), std::size_t{3});
        QCOMPARE(tonnes[0], 0.0);
        QCOMPARE(tonnes[1], 0.0);
        QCOMPARE(tonnes[2], 300.0);
    }

    void limitsRecentCumulativeTonnesToRequestedCount()
    {
        HaulHistoryStore store(QStringLiteral(":memory:"), QStringLiteral("test_mining_limits_recent"));
        for (int i = 0; i < 10; ++i) {
            store.recordSample(QStringLiteral("MT-001"), i * 1000, QStringLiteral("Hauling"), 300.0,
                                static_cast<double>(i) * 300.0);
        }
        const std::vector<double> tonnes = store.recentCumulativeTonnes(QStringLiteral("MT-001"), 3);
        QCOMPARE(tonnes.size(), std::size_t{3});
        QCOMPARE(tonnes[2], 2700.0);
    }
};

QTEST_MAIN(TestHaulHistoryStore)
#include "test_haul_history_store.moc"
