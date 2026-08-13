// SPDX-License-Identifier: GPL-3.0-or-later
#include "DropLayout.h"

#include <cmath>

namespace qttutorial::quick3d_physics {

std::vector<DropPosition> DropLayout::gridPositions(int count, double radius, double dropHeight)
{
    std::vector<DropPosition> positions;
    if (count <= 0 || radius <= 0.0) {
        return positions;
    }
    positions.reserve(static_cast<std::size_t>(count));

    const int columns = static_cast<int>(std::ceil(std::sqrt(static_cast<double>(count))));
    const int rows = static_cast<int>(std::ceil(static_cast<double>(count) / columns));
    const double spacing = radius * 2.5;

    for (int i = 0; i < count; ++i) {
        const int col = i % columns;
        const int row = i / columns;
        const double x = (col - (columns - 1) / 2.0) * spacing;
        const double z = (row - (rows - 1) / 2.0) * spacing;
        positions.push_back(DropPosition{x, dropHeight, z});
    }
    return positions;
}

} // namespace qttutorial::quick3d_physics
