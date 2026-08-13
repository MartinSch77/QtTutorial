// SPDX-License-Identifier: MIT
#include "KickerSimulation.h"
#include "KickerTypes.h"

#include <QTest>

#include <cmath>

using namespace qttutorial::games::kicker;

namespace {

std::array<RodInput, 4> noInput()
{
    return {};
}

} // namespace

class TestKickerSimulation : public QObject {
    Q_OBJECT
private slots:

    void ballReflectsOffSideWall()
    {
        MatchState state;
        state.ball.x = 32.0;
        state.ball.vx = 50.0;

        const MatchState next = KickerSimulation::step(state, 0.1, noInput());

        const double sideLimit = kHalfTableWidth - kBallRadius;
        QVERIFY(next.ball.x <= sideLimit + 1e-9);
        QCOMPARE(next.ball.x, sideLimit);
        QVERIFY2(next.ball.vx < 0.0, "velocity should reverse direction after hitting the side wall");
    }

    void ballReflectsOffOtherSideWallToo()
    {
        MatchState state;
        state.ball.x = -32.0;
        state.ball.vx = -50.0;

        const MatchState next = KickerSimulation::step(state, 0.1, noInput());

        const double sideLimit = kHalfTableWidth - kBallRadius;
        QCOMPARE(next.ball.x, -sideLimit);
        QVERIFY(next.ball.vx > 0.0);
    }

    void ballPassesThroughGoalMouthAndScores()
    {
        MatchState state;
        state.ball.x = 0.0; // inside the goal mouth (|x| <= kGoalHalfWidth)
        state.ball.z = -58.0;
        state.ball.vz = -100.0;

        const MatchState next = KickerSimulation::step(state, 0.1, noInput());

        QCOMPARE(next.score[0], 0);
        QCOMPARE(next.score[1], 1);
        QCOMPARE(next.ball.x, 0.0);
        QCOMPARE(next.ball.z, 0.0);
        QCOMPARE(next.ball.vx, 0.0);
        QCOMPARE(next.ball.vz, 0.0);
    }

    void ballIsBlockedByEndWallOutsideGoalMouth()
    {
        MatchState state;
        state.ball.x = 20.0; // outside the goal mouth (kGoalHalfWidth == 10)
        state.ball.z = -58.0;
        state.ball.vz = -100.0;

        const MatchState next = KickerSimulation::step(state, 0.1, noInput());

        const double endLimit = kHalfTableLength - kBallRadius;
        QCOMPARE(next.score[0], 0);
        QCOMPARE(next.score[1], 0);
        QCOMPARE(next.ball.z, -endLimit);
        QVERIFY2(next.ball.vz > 0.0, "ball should bounce back into the field, not through the end wall");
    }

    void scoringAtTheOppositeEndCreditsTeamA()
    {
        MatchState state;
        state.ball.x = 0.0;
        state.ball.z = 58.0;
        state.ball.vz = 100.0;

        const MatchState next = KickerSimulation::step(state, 0.1, noInput());

        QCOMPARE(next.score[0], 1);
        QCOMPARE(next.score[1], 0);
        QCOMPARE(next.ball.z, 0.0);
    }

    void positiveRodSpinAboveThresholdKicksBallInPositiveDirection()
    {
        MatchState state;
        state.ball.x = 0.0;
        state.ball.z = kRodZ[1]; // sitting right on rod 1's line, aligned with its middle figure

        auto inputs = noInput();
        inputs[1] = RodInput{0.0, 10.0}; // above kKickAngularThreshold (6.0)

        const MatchState next = KickerSimulation::step(state, 0.02, inputs);

        QVERIFY2(next.ball.vz > 0.0, "positive angular velocity should kick the ball in +z");
    }

    void negativeRodSpinAboveThresholdKicksBallInNegativeDirection()
    {
        MatchState state;
        state.ball.x = 0.0;
        state.ball.z = kRodZ[1];

        auto inputs = noInput();
        inputs[1] = RodInput{0.0, -10.0};

        const MatchState next = KickerSimulation::step(state, 0.02, inputs);

        QVERIFY2(next.ball.vz < 0.0, "negative angular velocity should kick the ball in -z");
    }

    void spinBelowThresholdDoesNotKickTheBall()
    {
        MatchState state;
        state.ball.x = 0.0;
        state.ball.z = kRodZ[1];

        auto inputs = noInput();
        inputs[1] = RodInput{0.0, 3.0}; // below kKickAngularThreshold (6.0)

        const MatchState next = KickerSimulation::step(state, 0.02, inputs);

        QCOMPARE(next.ball.vz, 0.0);
    }

    void ballOutsideKickRadiusIsUnaffectedBySpin()
    {
        MatchState state;
        state.ball.x = 0.0;
        state.ball.z = kRodZ[1] + 30.0; // far from rod 1

        auto inputs = noInput();
        inputs[1] = RodInput{0.0, 15.0};

        const MatchState next = KickerSimulation::step(state, 0.02, inputs);

        QCOMPARE(next.ball.vz, 0.0);
    }

    void slideClampsToRailBoundsForOutOfRangeInput()
    {
        auto inputs = noInput();
        inputs[0] = RodInput{1000.0, 0.0};
        const MatchState next = KickerSimulation::step(MatchState{}, 0.02, inputs);
        QCOMPARE(next.rods[0].slide, kRodSlideRange);

        inputs[0] = RodInput{-1000.0, 0.0};
        const MatchState next2 = KickerSimulation::step(MatchState{}, 0.02, inputs);
        QCOMPARE(next2.rods[0].slide, -kRodSlideRange);
    }

    void rotationClampsToBoundsForOutOfRangeSpin()
    {
        auto inputs = noInput();
        inputs[0] = RodInput{0.0, 1000.0};
        const MatchState next = KickerSimulation::step(MatchState{}, 1.0, inputs);
        QCOMPARE(next.rods[0].angularVelocity, kMaxAngularSpeed);
        QCOMPARE(next.rods[0].rotation, kMaxRotation);

        inputs[0] = RodInput{0.0, -1000.0};
        const MatchState next2 = KickerSimulation::step(MatchState{}, 1.0, inputs);
        QCOMPARE(next2.rods[0].angularVelocity, -kMaxAngularSpeed);
        QCOMPARE(next2.rods[0].rotation, -kMaxRotation);
    }

    void selectActiveRodPicksTheOneClosestToTheBall()
    {
        MatchState state;
        state.ball.z = kRodZ[0] + 1.0; // just past rod 0, closer to rod 0 than rod 1
        QCOMPARE(KickerSimulation::selectActiveRod(state, 0, 1), 0);

        state.ball.z = kRodZ[1] - 1.0; // closer to rod 1
        QCOMPARE(KickerSimulation::selectActiveRod(state, 0, 1), 1);
    }

    void rodsForSeatAssignsTwoRodsPerSeatInTwoPlayerMode()
    {
        QCOMPARE(KickerSimulation::rodsForSeat(0, 2), (std::array<int, 2>{0, 1}));
        QCOMPARE(KickerSimulation::rodsForSeat(1, 2), (std::array<int, 2>{2, 3}));
    }

    void rodsForSeatAssignsOneFixedRodPerSeatInFourPlayerMode()
    {
        QCOMPARE(KickerSimulation::rodsForSeat(0, 4), (std::array<int, 2>{0, -1}));
        QCOMPARE(KickerSimulation::rodsForSeat(3, 4), (std::array<int, 2>{3, -1}));
    }
};

QTEST_APPLESS_MAIN(TestKickerSimulation)
#include "test_kicker_simulation.moc"
