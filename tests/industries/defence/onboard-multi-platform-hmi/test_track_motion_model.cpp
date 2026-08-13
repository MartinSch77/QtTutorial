// SPDX-License-Identifier: MIT
#include "TrackMotionModel.h"

#include <QTest>

#include <cmath>

using namespace qttutorial::defence;

class TestTrackMotionModel : public QObject {
    Q_OBJECT
private slots:
    void producesInitialTracks()
    {
        TrackSimulator sim(40.0);
        QVERIFY(!sim.tracks().empty());
        for (const Track& track : sim.tracks()) {
            QVERIFY(!track.classification.isEmpty());
        }
    }

    void advancesPositionSmoothly()
    {
        TrackSimulator sim(40.0);
        const Track before = sim.tracks().front();
        sim.advance(1.0);
        const Track after = sim.tracks().front();

        const double distanceMoved = std::hypot(after.xKm - before.xKm, after.yKm - before.yKm);
        const double expectedMax = before.speedKmh / 3600.0 * 1.0 + 1e-6;
        QVERIFY(distanceMoved <= expectedMax);
        QVERIFY(distanceMoved >= 0.0);
    }

    void staysWithinSurveillanceArea()
    {
        TrackSimulator sim(40.0);
        for (int i = 0; i < 2000; ++i) {
            sim.advance(1.0);
        }
        for (const Track& track : sim.tracks()) {
            const double radius = std::hypot(track.xKm, track.yKm);
            QVERIFY(radius <= sim.areaRadiusKm() + 1e-6);
        }
    }

    void doesNotTeleport()
    {
        TrackSimulator sim(40.0);
        const Track before = sim.tracks()[1];
        sim.advance(0.1);
        const Track after = sim.tracks()[1];
        const double distanceMoved = std::hypot(after.xKm - before.xKm, after.yKm - before.yKm);
        QVERIFY(distanceMoved < 1.0);
    }
};

QTEST_MAIN(TestTrackMotionModel)
#include "test_track_motion_model.moc"
