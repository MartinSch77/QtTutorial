// SPDX-License-Identifier: MIT
#pragma once

#include "KickerTypes.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QList>

namespace qttutorial::games::kicker {

// Encode/decode helpers for the "state" and "input" TableMessage payloads
// (see the kicker README's "Network protocol" section for the exact wire
// shape). Kept Qt-Core-only and free of QObject/QTimer so they can be unit
// tested the same way as KickerSimulation.

[[nodiscard]] QJsonObject encodeMatchState(const MatchState& state);
[[nodiscard]] MatchState decodeMatchState(const QJsonObject& obj);

struct RodInputEntry {
    int rod = -1;
    RodInput input;
};

[[nodiscard]] QJsonObject encodeInputPayload(const QList<RodInputEntry>& entries);
[[nodiscard]] QList<RodInputEntry> decodeInputPayload(const QJsonObject& obj);

} // namespace qttutorial::games::kicker
