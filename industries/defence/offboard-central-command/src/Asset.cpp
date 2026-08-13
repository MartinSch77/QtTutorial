// SPDX-License-Identifier: MIT
#include "Asset.h"

namespace qttutorial::defence {

QString toString(AssetType type)
{
    switch (type) {
    case AssetType::Vehicle:
        return QStringLiteral("Vehicle");
    case AssetType::Drone:
        return QStringLiteral("Drone");
    case AssetType::Vessel:
        return QStringLiteral("Vessel");
    case AssetType::Installation:
        return QStringLiteral("Installation");
    }
    return QStringLiteral("Unknown");
}

} // namespace qttutorial::defence
