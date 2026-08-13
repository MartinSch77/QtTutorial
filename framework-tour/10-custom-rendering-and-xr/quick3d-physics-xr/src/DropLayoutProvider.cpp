// SPDX-License-Identifier: GPL-3.0-or-later
#include "DropLayoutProvider.h"
#include "DropLayout.h"

#include <QVector3D>

namespace qttutorial::quick3d_physics {

QVariantList DropLayoutProvider::gridPositions(int count, double radius, double dropHeight) const
{
    QVariantList result;
    for (const DropPosition& position : DropLayout::gridPositions(count, radius, dropHeight)) {
        result.append(QVariant::fromValue(QVector3D(static_cast<float>(position.x),
                                                      static_cast<float>(position.y),
                                                      static_cast<float>(position.z))));
    }
    return result;
}

} // namespace qttutorial::quick3d_physics
