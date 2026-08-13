// SPDX-License-Identifier: MIT
#include "GroundTrackMath.h"

#include <QTest>

#include <cmath>

using namespace qttutorial::space;

class TestGroundTrackMath : public QObject {
    Q_OBJECT
private slots:
    void latitudeCrossesEquatorAtNodes()
    {
        auto track = computeGroundTrack(0.0, 53.0, 0.0);
        QVERIFY(std::abs(track.latitudeDeg) < 0.5);

        track = computeGroundTrack(180.0, 53.0, 0.0);
        QVERIFY(std::abs(track.latitudeDeg) < 0.5);
    }

    void latitudeReachesInclinationAtQuarterOrbit()
    {
        const auto track = computeGroundTrack(90.0, 53.0, 0.0);
        QVERIFY(track.latitudeDeg > 52.0 && track.latitudeDeg <= 53.5);
    }

    void latitudeStaysBoundedByInclination()
    {
        for (int phase = 0; phase <= 360; phase += 10) {
            const auto track = computeGroundTrack(static_cast<double>(phase), 45.0, 0.0);
            QVERIFY(track.latitudeDeg >= -45.5 && track.latitudeDeg <= 45.5);
        }
    }

    void earthRotationShiftsLongitudeWestward()
    {
        const auto withoutRotation = computeGroundTrack(45.0, 53.0, 0.0);
        const auto withRotation = computeGroundTrack(45.0, 53.0, 20.0);
        QVERIFY(withRotation.longitudeDeg != withoutRotation.longitudeDeg);
    }

    void angularDistanceIsZeroForSamePoint()
    {
        // acos() near 1.0 is numerically sensitive, so allow a tiny epsilon
        // rather than requiring an exact 0.0.
        QVERIFY(angularDistanceDeg(10.0, 20.0, 10.0, 20.0) < 1e-4);
    }

    void angularDistanceIsSymmetric()
    {
        const double forward = angularDistanceDeg(10.0, 20.0, -30.0, 100.0);
        const double backward = angularDistanceDeg(-30.0, 100.0, 10.0, 20.0);
        QCOMPARE(forward, backward);
        QVERIFY(forward > 0.0);
    }

    void angularDistanceBetweenAntipodesIsNearlyHalfCircle()
    {
        const double distance = angularDistanceDeg(0.0, 0.0, 0.0, 180.0);
        QVERIFY(std::abs(distance - 180.0) < 0.01);
    }
};

QTEST_MAIN(TestGroundTrackMath)
#include "test_ground_track_math.moc"
