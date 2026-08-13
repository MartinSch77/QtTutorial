// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <expected>
#include <optional>
#include <vector>

namespace qttutorial::games::skat {

// A deliberately simplified bidding ("Reizen") state machine. Real Skat
// bidding is a two-player-at-a-time exchange where the higher-ranked player
// silently "asks" a value and the lower-ranked one "says yes" or passes,
// following a fixed value ladder derived from the game-value table. This
// class instead runs a straightforward round-robin numeric auction: each of
// the 3 active players, in turn, either raises strictly above the current
// highest bid or passes for good; the auction ends as soon as only one
// player has not passed, and that player becomes the declarer. See README
// "Simplifications" for the full rationale.
class Bidding {
public:
    explicit Bidding(std::array<int, 3> seatsInBiddingOrder, int baseline = 18);

    enum class ActionError { NotYourTurn, AlreadyFinished, BidTooLow, UnknownSeat };

    [[nodiscard]] std::expected<void, ActionError> submitBid(int seat, int amount);
    [[nodiscard]] std::expected<void, ActionError> submitPass(int seat);

    [[nodiscard]] bool isFinished() const;
    [[nodiscard]] int currentTurnSeat() const; // -1 once finished
    [[nodiscard]] int highestBid() const;
    [[nodiscard]] int highestBidder() const; // -1 if nobody has bid yet
    [[nodiscard]] int baseline() const { return m_baseline; }

    // Valid once isFinished() is true: the sole remaining player, and the
    // game value they committed to (their own highest bid, or the baseline
    // if the other two passed before anyone ever bid).
    [[nodiscard]] std::optional<int> declarerSeat() const;
    [[nodiscard]] int declarerValue() const;

private:
    int m_baseline;
    int m_highestBid;
    int m_highestBidder = -1;
    std::vector<int> m_activeOrder;
    std::size_t m_turnIndex = 0;
};

} // namespace qttutorial::games::skat
