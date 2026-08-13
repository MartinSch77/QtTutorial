// SPDX-License-Identifier: MIT
#pragma once

#include "KickerTypes.h"

#include <array>

namespace qttutorial::games::kicker {

// Pure, deterministic table-football physics: no QObject, no Qt Quick, no
// randomness. Given the same (state, dt, inputs), step() always produces
// the same new state, which is what makes it directly unit-testable.
class KickerSimulation {
public:
    // Advances the simulation by dt seconds, applying each rod's input,
    // moving/bouncing the ball, detecting kicks and goals.
    [[nodiscard]] static MatchState step(const MatchState& state, double dt,
                                          const std::array<RodInput, 4>& inputs);

    // In 2-player mode a single player controls both of their team's rods
    // with one mouse; the rod nearer the ball (by |z| distance) is the one
    // that should currently receive mouse input. Pure helper so the
    // UI/input layer and the tests can share the exact same rule.
    [[nodiscard]] static int selectActiveRod(const MatchState& state, int rodA, int rodB);

    // Seat-to-rod assignment, shared by host and clients so both agree on
    // who controls what without needing to exchange it over the network.
    // - humanSeatCount <= 2: seat 0 controls rods {0, 1} (Team A), seat 1
    //   controls rods {2, 3} (Team B); a missing seat 1 leaves Team B's
    //   rods idle (uncontrolled, holding position -- a deliberate
    //   simplification: no AI opponent).
    // - humanSeatCount >= 3: each seat i controls exactly one rod, rod i,
    //   fixed for the match; any rod beyond humanSeatCount is idle.
    [[nodiscard]] static std::array<int, 2> rodsForSeat(int logicalSeat, int humanSeatCount);
};

} // namespace qttutorial::games::kicker
