// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"

#include "TableMessage.h"

#include <QObject>

#include <array>
#include <optional>
#include <random>
#include <vector>

namespace qttutorial::maumau {

namespace common = ::qttutorial::games::common;

// The authoritative Mau-Mau state machine. Runs once, on the host: it consumes TableMessages
// (play_card/draw_card) tagged with a seat number and produces TableMessages (public_state,
// hand, action_error, notice) to send back out -- identically whether that seat belongs to a
// human sitting at the host, a network peer relayed through TableServer, or a seat this class
// itself plays via its built-in bot. See industries/games/maumau/README.md for the full message protocol.
class MauMauGame : public QObject {
    Q_OBJECT
public:
    explicit MauMauGame(QObject* parent = nullptr);

    void startNewGame(int seatCount, unsigned seed = std::random_device{}());

    // Deals from a caller-supplied, already-ordered deck instead of shuffling internally --
    // used by tests to set up specific hands/top-card scenarios deterministically. `deck` must
    // hold enough cards for `seatCount * 5` hand cards plus one discard-pile starter card; any
    // remainder becomes the draw pile.
    void startNewGameWithFixedDeck(int seatCount, std::vector<Card> deck);

    void setBotSeat(int seat, bool isBot);
    [[nodiscard]] bool isBotSeat(int seat) const;

    void handleMessage(int seat, const common::TableMessage& message);

    [[nodiscard]] int seatCount() const { return m_seatCount; }
    [[nodiscard]] int currentSeat() const { return m_currentSeat; }
    [[nodiscard]] bool isRoundOver() const { return m_winnerSeat.has_value(); }
    [[nodiscard]] std::optional<int> winnerSeat() const { return m_winnerSeat; }
    [[nodiscard]] const std::vector<Card>& handOf(int seat) const { return m_hands.at(static_cast<std::size_t>(seat)); }
    [[nodiscard]] Card topCard() const { return m_discard.back(); }
    [[nodiscard]] std::optional<Suit> wishedSuit() const { return m_wishedSuit; }
    [[nodiscard]] int drawPileCount() const { return static_cast<int>(m_drawPile.size()); }

signals:
    void seatMessage(int seat, common::TableMessage message);
    void broadcastMessage(common::TableMessage message);
    void gameStateChanged();
    void roundOver(int winnerSeat);

private:
    void dealFrom(std::vector<Card> deck);
    void publishPublicState();
    void publishHand(int seat);
    void applyPlay(int seat, const Card& card, std::optional<Suit> wish);
    Card drawOneCard();
    void giveDrawCards(int seat, int count);
    void runBotTurnsIfAny();
    void sendError(int seat, const QString& reason);
    [[nodiscard]] int nextSeat(int fromSeat, int step) const;

    int m_seatCount = 0;
    std::array<bool, 4> m_botSeat{};
    std::array<std::vector<Card>, 4> m_hands;
    std::vector<Card> m_drawPile;
    std::vector<Card> m_discard;
    int m_currentSeat = 0;
    std::optional<Suit> m_wishedSuit;
    std::optional<int> m_winnerSeat;
    std::mt19937 m_rng;
};

} // namespace qttutorial::maumau
