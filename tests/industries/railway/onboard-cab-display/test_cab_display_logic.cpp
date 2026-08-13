// SPDX-License-Identifier: MIT
#include "BrakingCurve.h"
#include "RouteProfile.h"
#include "TrainSimulator.h"

#include <QTest>

using namespace qttutorial::cab_display;

namespace {

RouteProfile makeTestRoute()
{
    std::vector<SpeedSegment> segments{
        {0.0, 1000.0, 120.0},
        {1000.0, 1200.0, 40.0},
        {1200.0, 3000.0, 120.0},
    };
    std::vector<Station> stations{{"TestHalt", 2000.0}};
    return RouteProfile(segments, stations);
}

} // namespace

class TestCabDisplayLogic : public QObject {
    Q_OBJECT
private slots:
    void permittedSpeedAtMatchesSegment()
    {
        const RouteProfile route = makeTestRoute();
        QCOMPARE(route.permittedSpeedAt(500.0), 120.0);
        QCOMPARE(route.permittedSpeedAt(1100.0), 40.0);
        QCOMPARE(route.permittedSpeedAt(2500.0), 120.0);
    }

    void nextRestrictionFindsTheNextLowerSpeedSegment()
    {
        const RouteProfile route = makeTestRoute();
        const auto restriction = route.nextRestriction(500.0);
        QVERIFY(restriction.has_value());
        QCOMPARE(restriction->distanceToStartM, 500.0);
        QCOMPARE(restriction->permittedSpeedKmh, 40.0);
    }

    void nextRestrictionIsEmptyWhenNoSlowerSegmentAhead()
    {
        const RouteProfile route = makeTestRoute();
        QVERIFY(!route.nextRestriction(1100.0).has_value());
    }

    void nextStationReturnsTheClosestOneAhead()
    {
        const RouteProfile route = makeTestRoute();
        const auto station = route.nextStation(500.0);
        QVERIFY(station.has_value());
        QCOMPARE(station->name, QStringLiteral("TestHalt"));
        QVERIFY(!route.nextStation(2500.0).has_value());
    }

    void brakingCurveReachesTargetExactlyAtRestriction()
    {
        const double safeSpeed = maxSafeSpeedKmh(0.0, 40.0, 0.8);
        QCOMPARE(safeSpeed, 40.0);
    }

    void brakingCurveAllowsHigherSpeedFurtherAway()
    {
        const double near = maxSafeSpeedKmh(50.0, 40.0, 0.8);
        const double far = maxSafeSpeedKmh(500.0, 40.0, 0.8);
        QVERIFY(far > near);
        QVERIFY(near >= 40.0);
    }

    void trainAcceleratesGraduallyRatherThanJumpingToPermittedSpeed()
    {
        TrainSimulator train(makeTestRoute());
        train.tick(0.1);
        QVERIFY(train.speedKmh() > 0.0);
        QVERIFY(train.speedKmh() < 120.0);
    }

    void trainSlowsBeforeEnteringLowerSpeedSegment()
    {
        TrainSimulator train(makeTestRoute());
        for (int i = 0; i < 6000 && train.positionM() < 900.0; ++i) {
            train.tick(0.1);
        }
        QVERIFY(train.speedKmh() <= train.permittedSpeedKmh() + 1.0);
        QVERIFY(train.permittedSpeedKmh() <= 120.0);
    }

    void trainOpensDoorsAtStationAndClosesAfterDwell()
    {
        TrainSimulator train(makeTestRoute());
        bool sawDoorsOpen = false;
        for (int i = 0; i < 20000; ++i) {
            train.tick(0.1);
            if (train.doorsOpen()) {
                sawDoorsOpen = true;
                break;
            }
        }
        QVERIFY(sawDoorsOpen);
        QCOMPARE(train.speedKmh(), 0.0);

        bool doorsClosedAgain = false;
        for (int i = 0; i < 2000; ++i) {
            train.tick(0.1);
            if (!train.doorsOpen()) {
                doorsClosedAgain = true;
                break;
            }
        }
        QVERIFY(doorsClosedAgain);
    }
};

QTEST_MAIN(TestCabDisplayLogic)
#include "test_cab_display_logic.moc"
