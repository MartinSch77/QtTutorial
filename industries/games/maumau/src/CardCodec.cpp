// SPDX-License-Identifier: MIT
#include "CardCodec.h"

namespace qttutorial::maumau {

QJsonObject toJson(const Card& card)
{
    QJsonObject object;
    object[QStringLiteral("suit")] = QString::fromUtf8(toString(card.suit).data(),
                                                         static_cast<int>(toString(card.suit).size()));
    object[QStringLiteral("rank")] = QString::fromUtf8(toString(card.rank).data(),
                                                         static_cast<int>(toString(card.rank).size()));
    return object;
}

std::optional<Card> cardFromJson(const QJsonObject& object)
{
    const auto suit = suitFromString(object.value(QStringLiteral("suit")).toString().toStdString());
    const auto rank = rankFromString(object.value(QStringLiteral("rank")).toString().toStdString());
    if (!suit || !rank) {
        return std::nullopt;
    }
    return Card{*suit, *rank};
}

QJsonArray toJson(const std::vector<Card>& cards)
{
    QJsonArray array;
    for (const Card& card : cards) {
        array.append(toJson(card));
    }
    return array;
}

} // namespace qttutorial::maumau
