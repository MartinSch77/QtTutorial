// SPDX-License-Identifier: MIT
#include "FlightHistoryStore.h"

#include <QTest>

using namespace qttutorial::avionics;

class TestFlightHistoryStore : public QObject {
    Q_OBJECT
private slots:
    void recordsAndRetrievesSamples()
    {
        FlightHistoryStore store(QStringLiteral("test_flight_history_store_connection"));
        QVERIFY(store.open(QStringLiteral(":memory:")));

        ParameterSample sample;
        sample.aircraftTail = QStringLiteral("N101QT");
        sample.parameterName = QStringLiteral("engine1_vibration_ips");
        sample.value = 0.21;
        sample.timestamp = QDateTime::currentDateTimeUtc();
        QVERIFY(store.recordSample(sample));

        const auto results = store.history(sample.aircraftTail, sample.parameterName, 10);
        QCOMPARE(results.size(), std::size_t(1));
        QCOMPARE(results.front().aircraftTail, sample.aircraftTail);
        QVERIFY(qFuzzyCompare(results.front().value, sample.value));
    }

    void filtersByAircraftAndParameter()
    {
        FlightHistoryStore store(QStringLiteral("test_flight_history_store_filter"));
        QVERIFY(store.open(QStringLiteral(":memory:")));

        ParameterSample a;
        a.aircraftTail = QStringLiteral("N101QT");
        a.parameterName = QStringLiteral("engine1_vibration_ips");
        a.value = 0.2;
        a.timestamp = QDateTime::currentDateTimeUtc();
        QVERIFY(store.recordSample(a));

        ParameterSample b;
        b.aircraftTail = QStringLiteral("N102QT");
        b.parameterName = QStringLiteral("engine1_vibration_ips");
        b.value = 0.9;
        b.timestamp = QDateTime::currentDateTimeUtc();
        QVERIFY(store.recordSample(b));

        const auto results = store.history(QStringLiteral("N101QT"), QStringLiteral("engine1_vibration_ips"), 10);
        QCOMPARE(results.size(), std::size_t(1));
        QCOMPARE(results.front().aircraftTail, QStringLiteral("N101QT"));
    }

    void respectsLimit()
    {
        FlightHistoryStore store(QStringLiteral("test_flight_history_store_limit"));
        QVERIFY(store.open(QStringLiteral(":memory:")));

        for (int i = 0; i < 5; ++i) {
            ParameterSample sample;
            sample.aircraftTail = QStringLiteral("N101QT");
            sample.parameterName = QStringLiteral("p");
            sample.value = i;
            sample.timestamp = QDateTime::currentDateTimeUtc().addSecs(i);
            QVERIFY(store.recordSample(sample));
        }

        const auto results = store.history(QStringLiteral("N101QT"), QStringLiteral("p"), 2);
        QCOMPARE(results.size(), std::size_t(2));
    }
};

QTEST_MAIN(TestFlightHistoryStore)
#include "test_flight_history_store.moc"
