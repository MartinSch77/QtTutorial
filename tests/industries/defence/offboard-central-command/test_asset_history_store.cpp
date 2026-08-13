// SPDX-License-Identifier: MIT
#include "AssetHistoryStore.h"

#include <QTest>

using namespace qttutorial::defence;

class TestAssetHistoryStore : public QObject {
    Q_OBJECT
private slots:
    void recordsAndRetrievesStatus()
    {
        AssetHistoryStore store(QStringLiteral("test_asset_history_store_connection"));
        QVERIFY(store.open(QStringLiteral(":memory:")));

        AssetHistoryRecord record;
        record.assetId = QStringLiteral("VEH-01");
        record.type = AssetType::Vehicle;
        record.xKm = 12.5;
        record.yKm = -4.5;
        record.headingDeg = 90.0;
        record.health = QStringLiteral("Nominal");
        record.timestamp = QDateTime::currentDateTimeUtc();
        QVERIFY(store.recordStatus(record));

        const auto results = store.history(record.assetId, 10);
        QCOMPARE(results.size(), std::size_t(1));
        QCOMPARE(results.front().assetId, record.assetId);
        QCOMPARE(toString(results.front().type), toString(AssetType::Vehicle));
        QVERIFY(qFuzzyCompare(results.front().xKm, record.xKm));
    }

    void filtersByAssetId()
    {
        AssetHistoryStore store(QStringLiteral("test_asset_history_store_filter"));
        QVERIFY(store.open(QStringLiteral(":memory:")));

        AssetHistoryRecord a;
        a.assetId = QStringLiteral("VEH-01");
        a.type = AssetType::Vehicle;
        a.health = QStringLiteral("Nominal");
        a.timestamp = QDateTime::currentDateTimeUtc();
        QVERIFY(store.recordStatus(a));

        AssetHistoryRecord b;
        b.assetId = QStringLiteral("DRN-01");
        b.type = AssetType::Drone;
        b.health = QStringLiteral("Caution");
        b.timestamp = QDateTime::currentDateTimeUtc();
        QVERIFY(store.recordStatus(b));

        const auto results = store.history(QStringLiteral("DRN-01"), 10);
        QCOMPARE(results.size(), std::size_t(1));
        QCOMPARE(results.front().assetId, QStringLiteral("DRN-01"));
        QCOMPARE(toString(results.front().type), toString(AssetType::Drone));
    }

    void respectsLimit()
    {
        AssetHistoryStore store(QStringLiteral("test_asset_history_store_limit"));
        QVERIFY(store.open(QStringLiteral(":memory:")));

        for (int i = 0; i < 5; ++i) {
            AssetHistoryRecord record;
            record.assetId = QStringLiteral("VEH-01");
            record.type = AssetType::Vehicle;
            record.health = QStringLiteral("Nominal");
            record.timestamp = QDateTime::currentDateTimeUtc().addSecs(i);
            QVERIFY(store.recordStatus(record));
        }

        const auto results = store.history(QStringLiteral("VEH-01"), 2);
        QCOMPARE(results.size(), std::size_t(2));
    }
};

QTEST_MAIN(TestAssetHistoryStore)
#include "test_asset_history_store.moc"
