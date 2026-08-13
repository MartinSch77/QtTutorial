// SPDX-License-Identifier: MIT
#include "TelemetryHistoryStore.h"

#include <QTest>

using namespace qttutorial::automotive::fleet;

class TestTelemetryHistoryStore : public QObject {
    Q_OBJECT
private slots:
    void opensInMemoryDatabase()
    {
        TelemetryHistoryStore store(QStringLiteral(":memory:"), QStringLiteral("test_opens_in_memory"));
        QVERIFY(store.isOpen());
    }

    void recordsAndQueriesSamplesInOrder()
    {
        TelemetryHistoryStore store(QStringLiteral(":memory:"), QStringLiteral("test_records_and_queries"));
        QVERIFY(store.isOpen());

        QVERIFY(store.recordSample(QStringLiteral("VH-001"), 1000, 40.0, 90.0));
        QVERIFY(store.recordSample(QStringLiteral("VH-001"), 2000, 45.0, 89.0));
        QVERIFY(store.recordSample(QStringLiteral("VH-001"), 3000, 50.0, 88.0));
        QVERIFY(store.recordSample(QStringLiteral("VH-002"), 1500, 10.0, 70.0));

        QCOMPARE(store.sampleCount(QStringLiteral("VH-001")), 3);

        const std::vector<double> speeds = store.recentSpeeds(QStringLiteral("VH-001"), 10);
        QCOMPARE(speeds.size(), std::size_t{3});
        QCOMPARE(speeds[0], 40.0);
        QCOMPARE(speeds[1], 45.0);
        QCOMPARE(speeds[2], 50.0);
    }

    void limitsRecentSpeedsToRequestedCount()
    {
        TelemetryHistoryStore store(QStringLiteral(":memory:"), QStringLiteral("test_limits_recent_speeds"));
        for (int i = 0; i < 10; ++i) {
            store.recordSample(QStringLiteral("VH-001"), i * 1000, static_cast<double>(i), 100.0);
        }
        const std::vector<double> speeds = store.recentSpeeds(QStringLiteral("VH-001"), 3);
        QCOMPARE(speeds.size(), std::size_t{3});
        QCOMPARE(speeds[2], 9.0);
    }
};

QTEST_MAIN(TestTelemetryHistoryStore)
#include "test_telemetry_history_store.moc"
