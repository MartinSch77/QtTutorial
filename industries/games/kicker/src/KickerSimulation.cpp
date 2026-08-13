// SPDX-License-Identifier: MIT
#include "KickerSimulation.h"

#include <algorithm>
#include <cmath>

namespace qttutorial::games::kicker {

namespace {

double clamp(double value, double lo, double hi)
{
    return std::clamp(value, lo, hi);
}

// Reflects the ball off the near end-wall segments outside the goal mouth
// on one end of the table, or lets it fully cross into the goal, updating
// score. `sign` is -1.0 for the Team-A-defended end (z == -kHalfTableLength)
// and +1.0 for the Team-B-defended end.
void resolveEndWall(BallState& ball, std::array<int, 2>& score, double sign)
{
    const double endLimit = kHalfTableLength - kBallRadius;
    const double z = ball.z * sign; // mirror so "beyond" always means "> endLimit"
    if (z <= endLimit) {
        return;
    }

    if (std::abs(ball.x) <= kGoalHalfWidth) {
        if (ball.z * sign >= kHalfTableLength) {
            // Fully crossed the goal line inside the goal mouth: a goal for
            // the *other* team (sign < 0 is Team A's own goal, so Team B
            // scores, and vice versa).
            score[sign < 0.0 ? 1 : 0] += 1;
            ball = BallState{};
        }
        return; // still travelling through the goal-mouth gap
    }

    ball.z = endLimit * sign;
    ball.vz = -ball.vz * kWallRestitution;
}

} // namespace

MatchState KickerSimulation::step(const MatchState& state, double dt, const std::array<RodInput, 4>& inputs)
{
    MatchState next = state;

    for (int i = 0; i < 4; ++i) {
        const RodInput& input = inputs[static_cast<std::size_t>(i)];
        RodState& rod = next.rods[static_cast<std::size_t>(i)];

        rod.slide = clamp(input.slideTarget, -kRodSlideRange, kRodSlideRange);
        rod.angularVelocity = clamp(input.spin, -kMaxAngularSpeed, kMaxAngularSpeed);
        rod.rotation = clamp(rod.rotation + rod.angularVelocity * dt, -kMaxRotation, kMaxRotation);
    }

    for (int i = 0; i < 4; ++i) {
        const RodState& rod = next.rods[static_cast<std::size_t>(i)];
        if (std::abs(rod.angularVelocity) < kKickAngularThreshold) {
            continue;
        }
        for (double offset : {-kFigureSpacing, 0.0, kFigureSpacing}) {
            const double dx = next.ball.x - (rod.slide + offset);
            const double dz = next.ball.z - kRodZ[static_cast<std::size_t>(i)];
            if (dx * dx + dz * dz <= kKickContactRadius * kKickContactRadius) {
                next.ball.vz += kKickImpulseScale * rod.angularVelocity;
                break;
            }
        }
    }

    BallState& ball = next.ball;
    const double frictionFactor = std::max(0.0, 1.0 - kBallFrictionPerSecond * dt);
    ball.vx *= frictionFactor;
    ball.vz *= frictionFactor;

    const double speed = std::hypot(ball.vx, ball.vz);
    if (speed > kMaxBallSpeed) {
        const double scale = kMaxBallSpeed / speed;
        ball.vx *= scale;
        ball.vz *= scale;
    }

    ball.x += ball.vx * dt;
    ball.z += ball.vz * dt;

    const double sideLimit = kHalfTableWidth - kBallRadius;
    if (ball.x < -sideLimit) {
        ball.x = -sideLimit;
        ball.vx = -ball.vx * kWallRestitution;
    } else if (ball.x > sideLimit) {
        ball.x = sideLimit;
        ball.vx = -ball.vx * kWallRestitution;
    }

    resolveEndWall(ball, next.score, -1.0);
    resolveEndWall(ball, next.score, 1.0);

    return next;
}

int KickerSimulation::selectActiveRod(const MatchState& state, int rodA, int rodB)
{
    const double zA = kRodZ[static_cast<std::size_t>(rodA)];
    const double zB = kRodZ[static_cast<std::size_t>(rodB)];
    const double distA = std::abs(state.ball.z - zA);
    const double distB = std::abs(state.ball.z - zB);
    return distA <= distB ? rodA : rodB;
}

std::array<int, 2> KickerSimulation::rodsForSeat(int logicalSeat, int humanSeatCount)
{
    if (humanSeatCount <= 2) {
        return logicalSeat == 0 ? std::array<int, 2>{0, 1} : std::array<int, 2>{2, 3};
    }
    return {logicalSeat, -1};
}

} // namespace qttutorial::games::kicker
