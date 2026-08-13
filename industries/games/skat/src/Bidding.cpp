// SPDX-License-Identifier: MIT
#include "Bidding.h"

#include <algorithm>

namespace qttutorial::games::skat {

Bidding::Bidding(std::array<int, 3> seatsInBiddingOrder, int baseline)
    : m_baseline(baseline)
    , m_highestBid(baseline - 1)
    , m_activeOrder(seatsInBiddingOrder.begin(), seatsInBiddingOrder.end())
{
}

bool Bidding::isFinished() const
{
    return m_activeOrder.size() <= 1;
}

int Bidding::currentTurnSeat() const
{
    return isFinished() ? -1 : m_activeOrder[m_turnIndex];
}

int Bidding::highestBid() const
{
    return m_highestBidder == -1 ? 0 : m_highestBid;
}

int Bidding::highestBidder() const
{
    return m_highestBidder;
}

std::optional<int> Bidding::declarerSeat() const
{
    if (!isFinished()) {
        return std::nullopt;
    }
    return m_activeOrder.front();
}

int Bidding::declarerValue() const
{
    return m_highestBidder == -1 ? m_baseline : m_highestBid;
}

std::expected<void, Bidding::ActionError> Bidding::submitBid(int seat, int amount)
{
    if (isFinished()) {
        return std::unexpected(ActionError::AlreadyFinished);
    }
    if (seat != currentTurnSeat()) {
        return std::unexpected(ActionError::NotYourTurn);
    }
    if (amount <= m_highestBid) {
        return std::unexpected(ActionError::BidTooLow);
    }

    m_highestBid = amount;
    m_highestBidder = seat;
    m_turnIndex = (m_turnIndex + 1) % m_activeOrder.size();
    return {};
}

std::expected<void, Bidding::ActionError> Bidding::submitPass(int seat)
{
    if (isFinished()) {
        return std::unexpected(ActionError::AlreadyFinished);
    }
    if (seat != currentTurnSeat()) {
        return std::unexpected(ActionError::NotYourTurn);
    }

    // seat == currentTurnSeat() was just checked above, so the erased
    // element is always at m_turnIndex itself: everything after it shifts
    // left by one, meaning m_turnIndex already points at the next player in
    // rotation -- unless the passer was last in the vector, in which case
    // it must wrap back to 0.
    const auto it = std::ranges::find(m_activeOrder, seat);
    m_activeOrder.erase(it);

    if (m_activeOrder.empty()) {
        return {};
    }
    m_turnIndex %= m_activeOrder.size();
    return {};
}

} // namespace qttutorial::games::skat
