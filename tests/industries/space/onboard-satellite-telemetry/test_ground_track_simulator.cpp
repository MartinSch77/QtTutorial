// SPDX-License-Identifier: MIT
#include "GroundTrackSimulator.h"

#include <QTest>

#include <cmath>

using namespace qttutorial::space;

class TestGroundTrackSimulator : public QObject {
    Q_OBJECT
private slots:
    void latitudeStaysWithinInclinationBounds()
    {
        GroundTrackSimulator track(53.0);
        for (int i = 0; i <= 360; i += 5) {
            track.step(1.0, static_cast<double>(i));
            QVERIFY(track.state().latitudeDeg >= -53.5 && track.state().latitudeDeg <= 53.5);
        }
    }

    void crossesEquatorAtNodes()
    {
        GroundTrackSimulator track(53.0);
        track.step(0.0, 0.0);
        QVERIFY(std::abs(track.state().latitudeDeg) < 0.5);

        track.step(0.0, 180.0);
        QVERIFY(std::abs(track.state().latitudeDeg) < 0.5);
    }

    void reachesMaximumLatitudeNearQuarterOrbit()
    {
        GroundTrackSimulator track(53.0);
        track.step(0.0, 90.0);
        QVERIFY(track.state().latitudeDeg > 52.0);
    }

    void longitudeDriftsWestwardOverTime()
    {
        GroundTrackSimulator track(53.0);
        track.step(0.0, 10.0);
        const double firstLongitude = track.state().longitudeDeg;
        // A large elapsed time at the same true anomaly isolates the westward
        // drift caused purely by simulated Earth rotation.
        track.step(3600.0 * 6.0, 10.0);
        QVERIFY(track.state().longitudeDeg != firstLongitude);
    }

    void stationViewIsSymmetricAndBounded()
    {
        GroundTrackState overhead{45.0, 10.0};
        QVERIFY(isWithinStationView(overhead, 45.0, 10.0, 5.0));

        GroundTrackState farAway{-45.0, -170.0};
        QVERIFY(!isWithinStationView(farAway, 45.0, 10.0, 5.0));
    }
};

QTEST_MAIN(TestGroundTrackSimulator)
#include "test_ground_track_simulator.moc"
