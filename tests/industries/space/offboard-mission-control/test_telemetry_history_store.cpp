// SPDX-License-Identifier: MIT
#include "TelemetryHistoryStore.h"

#include <QTest>

using namespace qttutorial::space;

class TestTelemetryHistoryStore : public QObject {
    Q_OBJECT
private slots:
    void recordsAndRetrievesTelemetry()
    {
        TelemetryHistoryStore store(QStringLiteral("test_telemetry_history_store_connection"));
        QVERIFY(store.open(QStringLiteral(":memory:")));

        TelemetryRecord record;
        record.satelliteName = QStringLiteral("AURORA-1");
        record.batteryPercent = 72.5;
        record.phaseDeg = 123.4;
        record.inEclipse = true;
        record.health = QStringLiteral("Nominal");
        record.timestamp = QDateTime::currentDateTimeUtc();
        QVERIFY(store.recordTelemetry(record));

        const auto results = store.history(record.satelliteName, 10);
        QCOMPARE(results.size(), std::size_t(1));
        QCOMPARE(results.front().satelliteName, record.satelliteName);
        QVERIFY(qFuzzyCompare(results.front().batteryPercent, record.batteryPercent));
        QCOMPARE(results.front().inEclipse, true);
    }

    void filtersBySatelliteName()
    {
        TelemetryHistoryStore store(QStringLiteral("test_telemetry_history_store_filter"));
        QVERIFY(store.open(QStringLiteral(":memory:")));

        TelemetryRecord a;
        a.satelliteName = QStringLiteral("AURORA-1");
        a.batteryPercent = 50.0;
        a.timestamp = QDateTime::currentDateTimeUtc();
        a.health = QStringLiteral("Nominal");
        QVERIFY(store.recordTelemetry(a));

        TelemetryRecord b;
        b.satelliteName = QStringLiteral("AURORA-2");
        b.batteryPercent = 20.0;
        b.timestamp = QDateTime::currentDateTimeUtc();
        b.health = QStringLiteral("Caution");
        QVERIFY(store.recordTelemetry(b));

        const auto results = store.history(QStringLiteral("AURORA-2"), 10);
        QCOMPARE(results.size(), std::size_t(1));
        QCOMPARE(results.front().satelliteName, QStringLiteral("AURORA-2"));
    }

    void respectsLimit()
    {
        TelemetryHistoryStore store(QStringLiteral("test_telemetry_history_store_limit"));
        QVERIFY(store.open(QStringLiteral(":memory:")));

        for (int i = 0; i < 5; ++i) {
            TelemetryRecord record;
            record.satelliteName = QStringLiteral("AURORA-1");
            record.batteryPercent = i;
            record.health = QStringLiteral("Nominal");
            record.timestamp = QDateTime::currentDateTimeUtc().addSecs(i);
            QVERIFY(store.recordTelemetry(record));
        }

        const auto results = store.history(QStringLiteral("AURORA-1"), 2);
        QCOMPARE(results.size(), std::size_t(2));
    }
};

QTEST_MAIN(TestTelemetryHistoryStore)
#include "test_telemetry_history_store.moc"
