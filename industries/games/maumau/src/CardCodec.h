// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"

#include <QJsonArray>
#include <QJsonObject>

#include <optional>
#include <vector>

namespace qttutorial::maumau {

[[nodiscard]] QJsonObject toJson(const Card& card);
[[nodiscard]] std::optional<Card> cardFromJson(const QJsonObject& object);

[[nodiscard]] QJsonArray toJson(const std::vector<Card>& cards);

} // namespace qttutorial::maumau
