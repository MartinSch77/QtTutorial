// SPDX-License-Identifier: MIT
#include "GroundStationTracker.h"

#include <QTest>

using namespace qttutorial::space;

class TestGroundStationTracker : public QObject {
    Q_OBJECT
private slots:
    void hasAFixedSetOfStations()
    {
        GroundStationTracker tracker;
        QVERIFY(!tracker.stations().empty());
        for (const auto& station : tracker.stations()) {
            QVERIFY(!station.name.isEmpty());
            QVERIFY(station.visibilityRadiusDeg > 0.0);
        }
    }

    void reportsInContactWhenAlreadyOverhead()
    {
        GroundStationTracker tracker;
        const GroundStation& station = tracker.stations().front();

        SatelliteState satellite;
        satellite.name = QStringLiteral("TEST-1");
        satellite.latitudeDeg = station.latitudeDeg;
        satellite.longitudeDeg = station.longitudeDeg;
        satellite.inclinationDeg = 53.0;

        const auto window = tracker.nextContact(satellite, 95.0);
        QVERIFY(window.has_value());
        QVERIFY(window->inContactNow);
        QCOMPARE(window->stationName, station.name);
        QCOMPARE(window->minutesUntilNextContact, 0.0);
    }

    void predictsAFutureContactWhenNotOverhead()
    {
        GroundStationTracker tracker;

        SatelliteState satellite;
        satellite.name = QStringLiteral("TEST-2");
        satellite.phaseDeg = 0.0;
        satellite.inclinationDeg = 97.4; // near-polar: passes over most latitudes
        satellite.latitudeDeg = 0.0;
        satellite.longitudeDeg = 0.0;

        const auto window = tracker.nextContact(satellite, 96.0);
        QVERIFY(window.has_value());
        if (!window->inContactNow) {
            QVERIFY(window->minutesUntilNextContact > 0.0);
        }
    }
};

QTEST_MAIN(TestGroundStationTracker)
#include "test_ground_station_tracker.moc"
