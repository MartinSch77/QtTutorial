// SPDX-License-Identifier: MIT
#include "Bot.h"

#include "RufspielRules.h"

#include <cassert>

namespace qttutorial::games::schafkopf {

Bot::Bot(std::mt19937::result_type seed)
    : m_rng(seed)
{
}

std::optional<Suit> Bot::chooseCall(const std::vector<Card>& hand)
{
    const std::vector<Suit> options = legalCallOptions(hand);
    if (options.empty()) {
        return std::nullopt;
    }
    return options.front();
}

Card Bot::choosePlay(const std::vector<Card>& hand, const std::vector<Card>& trickSoFar)
{
    const std::vector<Card> options = legalPlays(hand, trickSoFar);
    assert(!options.empty());
    std::uniform_int_distribution<std::size_t> dist(0, options.size() - 1);
    return options[dist(m_rng)];
}

} // namespace qttutorial::games::schafkopf
