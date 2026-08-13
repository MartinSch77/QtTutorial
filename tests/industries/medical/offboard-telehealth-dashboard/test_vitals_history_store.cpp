// SPDX-License-Identifier: MIT
#include "VitalsHistoryStore.h"

#include <QTest>

using namespace qttutorial::medical::telehealth;

class TestVitalsHistoryStore : public QObject {
    Q_OBJECT
private slots:
    void opensInMemoryDatabase()
    {
        VitalsHistoryStore store(QStringLiteral(":memory:"), QStringLiteral("test_opens_in_memory"));
        QVERIFY(store.isOpen());
    }

    void recordsAndQueriesSamplesInOrder()
    {
        VitalsHistoryStore store(QStringLiteral(":memory:"), QStringLiteral("test_records_and_queries"));
        QVERIFY(store.recordSample(QStringLiteral("PT-01"), 1000, 72.0, 97.0, 118.0, 75.0));
        QVERIFY(store.recordSample(QStringLiteral("PT-01"), 2000, 80.0, 96.0, 120.0, 76.0));
        QVERIFY(store.recordSample(QStringLiteral("PT-02"), 1500, 65.0, 98.0, 110.0, 70.0));

        QCOMPARE(store.sampleCount(QStringLiteral("PT-01")), 2);
        const std::vector<double> rates = store.recentHeartRates(QStringLiteral("PT-01"), 10);
        QCOMPARE(rates.size(), std::size_t{2});
        QCOMPARE(rates[0], 72.0);
        QCOMPARE(rates[1], 80.0);
    }
};

QTEST_MAIN(TestVitalsHistoryStore)
#include "test_vitals_history_store.moc"
