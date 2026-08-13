// SPDX-License-Identifier: MIT
#pragma once

#include <QDateTime>
#include <QString>

namespace qttutorial::defence {

enum class AssetType { Vehicle, Drone, Vessel, Installation };

[[nodiscard]] QString toString(AssetType type);

// A single tracked friendly/simulated asset shown on the tactical map and
// status table: identity, position, heading and a health classification.
// There is no weapons loadout, no target list, and no fire-control field
// anywhere in this struct - it models only status and last-known position.
struct Asset {
    QString id;
    AssetType type = AssetType::Vehicle;
    double xKm = 0.0;
    double yKm = 0.0;
    double headingDeg = 0.0;
    QString health = QStringLiteral("Nominal");
    QDateTime lastUpdated;
};

} // namespace qttutorial::defence
