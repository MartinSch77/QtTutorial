// SPDX-License-Identifier: MIT
#pragma once

#include <array>

namespace qttutorial::games::kicker {

// Table and rod geometry, in centimetres, laid out on the table plane: x is
// the "slide" axis (left/right across the table, what the mouse's x
// position maps to), z is the goal-to-goal axis. A real foosball table has
// more rods (typically 5 per full-size table); this tutorial deliberately
// simplifies to 2 rods per team -- one "defense" rod near the team's own
// goal, one "attack" rod near midfield -- 4 rods and 12 figures in total.
inline constexpr double kHalfTableWidth = 34.0;
inline constexpr double kHalfTableLength = 60.0;
inline constexpr double kGoalHalfWidth = 10.0;

inline constexpr double kBallRadius = 1.5;
inline constexpr double kFigureRadius = 2.5;
inline constexpr double kFigureSpacing = 9.0; // distance between adjacent figures on a rod
inline constexpr double kKickContactRadius = kBallRadius + kFigureRadius;

// A rod's slide range is clamped so its outermost figures never cross the
// side walls.
inline constexpr double kRodSlideRange = kHalfTableWidth - kFigureSpacing;

// Rotation is clamped to a bounded back-and-forth swing (rather than a
// freely spinning rod) -- a deliberate simplification that keeps "rotation"
// a well-defined, boundable quantity for both the mouse mapping and the
// unit tests.
inline constexpr double kMaxRotation = 3.14159265358979323846; // +/- 180 degrees
inline constexpr double kMaxAngularSpeed = 20.0;                // rad/s
inline constexpr double kKickAngularThreshold = 6.0;            // rad/s

inline constexpr double kKickImpulseScale = 0.6;
inline constexpr double kBallFrictionPerSecond = 0.35; // fraction of speed lost per second
inline constexpr double kWallRestitution = 0.8;
inline constexpr double kMaxBallSpeed = 220.0;

enum class TeamId { A = 0, B = 1 };

// Fixed z-position of each of the 4 rods. Rods 0-1 belong to Team A
// (defending the goal at z == -kHalfTableLength), rods 2-3 to Team B
// (defending the goal at z == +kHalfTableLength).
inline constexpr std::array<double, 4> kRodZ = {
    -kHalfTableLength * 0.7,
    -kHalfTableLength * 0.25,
    kHalfTableLength * 0.25,
    kHalfTableLength * 0.7,
};

[[nodiscard]] constexpr TeamId teamOfRod(int rodIndex)
{
    return rodIndex < 2 ? TeamId::A : TeamId::B;
}

struct BallState {
    double x = 0.0;
    double z = 0.0;
    double vx = 0.0;
    double vz = 0.0;
};

struct RodState {
    double slide = 0.0;         // clamped to [-kRodSlideRange, +kRodSlideRange]
    double rotation = 0.0;      // clamped to [-kMaxRotation, +kMaxRotation]
    double angularVelocity = 0.0; // rad/s, clamped to [-kMaxAngularSpeed, +kMaxAngularSpeed]
};

// A player's per-tick command for one rod. slideTarget is an absolute
// position on the slide axis (mouse position mapped 1:1); spin is a
// commanded angular velocity (mouse horizontal speed mapped 1:1).
struct RodInput {
    double slideTarget = 0.0;
    double spin = 0.0;
};

struct MatchState {
    BallState ball;
    std::array<RodState, 4> rods{};
    std::array<int, 2> score{0, 0}; // score[0] = Team A goals, score[1] = Team B goals
};

} // namespace qttutorial::games::kicker
