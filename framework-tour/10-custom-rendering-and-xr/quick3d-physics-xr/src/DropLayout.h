// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <vector>

namespace qttutorial::quick3d_physics {

struct DropPosition {
    double x;
    double y;
    double z;
};

// Pure, testable helper: lays `count` falling bodies out on a grid above the
// floor, spaced at least 2 * radius apart in x/z. This has nothing to do
// with QML/Quick3D/PhysX directly - it exists because a naive "just stack
// them at the same spot" starting layout makes PhysX resolve the initial
// interpenetration by shoving bodies apart violently on the very first
// simulation step, which is an easy and non-obvious bug to introduce in a
// falling-bodies demo like this one.
class DropLayout {
public:
    [[nodiscard]] static std::vector<DropPosition> gridPositions(int count, double radius,
                                                                  double dropHeight);
};

} // namespace qttutorial::quick3d_physics
