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

// A route with two consecutive speed drops so upcomingRestrictions() has
// more than one entry to find within a single lookahead window.
RouteProfile makeMultiRestrictionRoute()
{
    std::vector<SpeedSegment> segments{
        {0.0, 500.0, 140.0},
        {500.0, 800.0, 100.0},
        {800.0, 1100.0, 140.0},
        {1100.0, 1400.0, 60.0},
        {1400.0, 5000.0, 140.0},
    };
    return RouteProfile(segments);
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

    void gradientPercentAtMatchesSegmentAndDefaultsToZero()
    {
        std::vector<SpeedSegment> segments{
            {0.0, 1000.0, 120.0, -6.0},
            {1000.0, 2000.0, 40.0},
        };
        const RouteProfile route(segments);
        QCOMPARE(route.gradientPercentAt(500.0), -6.0);
        QCOMPARE(route.gradientPercentAt(1500.0), 0.0);
    }

    void upcomingRestrictionsFindsEachSpeedDropInOrderWithinLookahead()
    {
        const RouteProfile route = makeMultiRestrictionRoute();
        const auto restrictions = route.upcomingRestrictions(0.0, 2000.0);
        QCOMPARE(restrictions.size(), std::size_t(2));
        QCOMPARE(restrictions[0].distanceToStartM, 500.0);
        QCOMPARE(restrictions[0].permittedSpeedKmh, 100.0);
        QCOMPARE(restrictions[1].distanceToStartM, 1100.0);
        QCOMPARE(restrictions[1].permittedSpeedKmh, 60.0);
    }

    void upcomingRestrictionsRespectsTheLookaheadWindow()
    {
        const RouteProfile route = makeMultiRestrictionRoute();
        const auto restrictions = route.upcomingRestrictions(0.0, 600.0);
        QCOMPARE(restrictions.size(), std::size_t(1));
        QCOMPARE(restrictions[0].permittedSpeedKmh, 100.0);
    }

    void upcomingRestrictionsRespectsMaxCount()
    {
        const RouteProfile route = makeMultiRestrictionRoute();
        const auto restrictions = route.upcomingRestrictions(0.0, 2000.0, 1);
        QCOMPARE(restrictions.size(), std::size_t(1));
        QCOMPARE(restrictions[0].permittedSpeedKmh, 100.0);
    }

    void upcomingRestrictionsIsEmptyWhenRouteIsClearAhead()
    {
        const RouteProfile route = makeTestRoute();
        const auto restrictions = route.upcomingRestrictions(1200.0, 1000.0);
        QVERIFY(restrictions.empty());
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

    void brakeWarningLatchesOnAboveOnMarginAndStaysOnUntilOffMargin()
    {
        // Below the "on" margin: never latches on.
        QVERIFY(!nextBrakeWarningState(false, 42.0, 40.0, 3.0, 0.5));
        // Above the "on" margin: latches on.
        QVERIFY(nextBrakeWarningState(false, 44.0, 40.0, 3.0, 0.5));
        // Once active, dropping back within the (wider) idle band but still
        // above the "off" margin keeps it latched — this is the hysteresis
        // that avoids flicker right at the threshold.
        QVERIFY(nextBrakeWarningState(true, 41.0, 40.0, 3.0, 0.5));
        // Only clears once at or below the "off" margin.
        QVERIFY(!nextBrakeWarningState(true, 40.3, 40.0, 3.0, 0.5));
    }

    void trainStaysWithinMarginOnFlatOrUphillApproachToARestriction()
    {
        // On flat (or uphill) track the simulator's actual deceleration
        // authority matches (or exceeds) what the flat-track braking curve
        // assumed, so the brake warning must never latch on the ordinary
        // approach to a restriction.
        std::vector<SpeedSegment> segments{
            {0.0, 1500.0, 140.0},
            {1500.0, 5000.0, 40.0},
        };
        RouteProfile route(segments);
        TrainSimulator train(route);
        for (int i = 0; i < 4000 && train.positionM() < 1600.0; ++i) {
            train.tick(0.1);
            QVERIFY(!train.brakeWarningActive());
        }
    }

    void trainBrakeWarningLatchesOnASteepDownhillApproachToARestriction()
    {
        // The braking-curve function assumes a flat track. A steep downhill
        // grade immediately before a restriction gives the train genuinely
        // less deceleration authority than the curve assumed when it set
        // the permitted speed, so real speed can plausibly run ahead of the
        // (flat-track) target — the realistic condition the brake-warning
        // indication exists to catch, not a discretisation artefact.
        std::vector<SpeedSegment> segments{
            {0.0, 1500.0, 140.0, -6.0}, // steep -6% grade approaching the restriction
            {1500.0, 5000.0, 40.0, 0.0},
        };
        RouteProfile route(segments);
        TrainSimulator train(route);

        bool sawWarning = false;
        for (int i = 0; i < 4000 && train.positionM() < 1600.0; ++i) {
            train.tick(0.1);
            if (train.brakeWarningActive()) {
                sawWarning = true;
            }
        }
        QVERIFY(sawWarning);

        // And it must clear again once the train has settled at (or below)
        // the permitted speed for the segment it is now in.
        for (int i = 0; i < 2000; ++i) {
            train.tick(0.1);
        }
        QVERIFY(!train.brakeWarningActive());
    }

    void trainExposesUpcomingRestrictionsForThePlanningStrip()
    {
        TrainSimulator train(makeMultiRestrictionRoute());
        const QVariantList restrictions = train.upcomingRestrictions();
        QVERIFY(!restrictions.isEmpty());
        const QVariantMap first = restrictions.first().toMap();
        QVERIFY(first.contains(QStringLiteral("distanceM")));
        QVERIFY(first.contains(QStringLiteral("speedKmh")));
        QCOMPARE(first.value(QStringLiteral("speedKmh")).toDouble(), 100.0);
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
