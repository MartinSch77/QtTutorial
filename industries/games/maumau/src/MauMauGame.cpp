// SPDX-License-Identifier: MIT
#include "MauMauGame.h"

#include "CardCodec.h"
#include "Deck.h"
#include "MoveValidator.h"

#include <QJsonArray>

#include <algorithm>
#include <iterator>

namespace qttutorial::maumau {

namespace {
constexpr int kHandSize = 5;
}

MauMauGame::MauMauGame(QObject* parent)
    : QObject(parent)
{
}

void MauMauGame::startNewGame(int seatCount, unsigned seed)
{
    m_rng.seed(seed);
    std::vector<Card> deck = fullDeck();
    shuffle(deck, m_rng);
    startNewGameWithFixedDeck(seatCount, std::move(deck));
}

void MauMauGame::startNewGameWithFixedDeck(int seatCount, std::vector<Card> deck)
{
    m_seatCount = seatCount;
    m_botSeat.fill(false);
    for (auto& hand : m_hands) {
        hand.clear();
    }
    m_wishedSuit.reset();
    m_winnerSeat.reset();
    dealFrom(std::move(deck));
}

void MauMauGame::setBotSeat(int seat, bool isBot)
{
    m_botSeat.at(static_cast<std::size_t>(seat)) = isBot;
}

bool MauMauGame::isBotSeat(int seat) const
{
    return m_botSeat.at(static_cast<std::size_t>(seat));
}

void MauMauGame::dealFrom(std::vector<Card> deck)
{
    auto it = deck.begin();
    for (int i = 0; i < kHandSize; ++i) {
        for (int seat = 0; seat < m_seatCount; ++seat) {
            m_hands[static_cast<std::size_t>(seat)].push_back(*it++);
        }
    }

    m_discard.clear();
    // The very first flip starts the discard pile but never triggers a special effect: nobody
    // "played" it, so there is no preceding player to skip and no next player has yet had a
    // turn to draw into. This is our documented, deliberate house-rule choice (see README).
    m_discard.push_back(*it++);
    m_drawPile.assign(it, deck.end());

    m_currentSeat = 0;
    m_wishedSuit.reset();
    m_winnerSeat.reset();

    for (int seat = 0; seat < m_seatCount; ++seat) {
        publishHand(seat);
    }
    publishPublicState();
    emit gameStateChanged();

    runBotTurnsIfAny();
}

int MauMauGame::nextSeat(int fromSeat, int step) const
{
    return ((fromSeat + step) % m_seatCount + m_seatCount) % m_seatCount;
}

Card MauMauGame::drawOneCard()
{
    if (m_drawPile.empty()) {
        // Recycle the discard pile, keeping only its current top card in place so the discard
        // pile still shows what was last legally played.
        Card top = m_discard.back();
        m_discard.pop_back();
        m_drawPile = std::move(m_discard);
        shuffle(m_drawPile, m_rng);
        m_discard.clear();
        m_discard.push_back(top);
    }
    Card card = m_drawPile.back();
    m_drawPile.pop_back();
    return card;
}

void MauMauGame::giveDrawCards(int seat, int count)
{
    for (int i = 0; i < count; ++i) {
        m_hands.at(static_cast<std::size_t>(seat)).push_back(drawOneCard());
    }
}

void MauMauGame::sendError(int seat, const QString& reason)
{
    common::TableMessage message;
    message.type = QStringLiteral("action_error");
    message.seat = seat;
    message.payload[QStringLiteral("reason")] = reason;
    emit seatMessage(seat, message);
}

void MauMauGame::handleMessage(int seat, const common::TableMessage& message)
{
    if (m_winnerSeat.has_value()) {
        sendError(seat, QStringLiteral("round_over"));
        return;
    }
    if (seat != m_currentSeat) {
        sendError(seat, QStringLiteral("not_your_turn"));
        return;
    }

    if (message.type == QStringLiteral("play_card")) {
        const auto card = cardFromJson(message.payload);
        if (!card) {
            sendError(seat, QStringLiteral("malformed_card"));
            return;
        }
        auto& hand = m_hands.at(static_cast<std::size_t>(seat));
        const auto handIt = std::ranges::find(hand, *card);
        if (handIt == hand.end()) {
            sendError(seat, QStringLiteral("card_not_in_hand"));
            return;
        }

        const DiscardState state{topCard(), m_wishedSuit};
        if (!isLegalPlay(*card, state)) {
            sendError(seat, QStringLiteral("illegal_card"));
            return;
        }

        std::optional<Suit> wish;
        if (card->rank == Rank::Jack) {
            const QString wishText = message.payload.value(QStringLiteral("wish")).toString();
            wish = suitFromString(wishText.toStdString());
            if (!wish) {
                sendError(seat, QStringLiteral("missing_wish"));
                return;
            }
        }

        hand.erase(handIt);
        applyPlay(seat, *card, wish);
        runBotTurnsIfAny();
    } else if (message.type == QStringLiteral("draw_card")) {
        giveDrawCards(seat, 1);
        m_currentSeat = nextSeat(seat, 1);
        publishHand(seat);
        publishPublicState();
        emit gameStateChanged();
        runBotTurnsIfAny();
    } else {
        sendError(seat, QStringLiteral("unknown_message_type"));
    }
}

void MauMauGame::applyPlay(int seat, const Card& card, std::optional<Suit> wish)
{
    m_discard.push_back(card);

    if (card.rank == Rank::Jack) {
        m_wishedSuit = wish;
    } else {
        m_wishedSuit.reset();
    }

    if (m_hands.at(static_cast<std::size_t>(seat)).empty()) {
        m_winnerSeat = seat;
        publishHand(seat);
        publishPublicState();
        emit gameStateChanged();
        emit roundOver(seat);
        return;
    }

    switch (effectOf(card.rank)) {
    case SpecialEffect::DrawTwo: {
        const int affected = nextSeat(seat, 1);
        giveDrawCards(affected, 2);
        common::TableMessage notice;
        notice.type = QStringLiteral("notice");
        notice.payload[QStringLiteral("kind")] = QStringLiteral("draw_two");
        notice.payload[QStringLiteral("seat")] = affected;
        emit broadcastMessage(notice);
        publishHand(affected);
        m_currentSeat = nextSeat(seat, 2);
        break;
    }
    case SpecialEffect::Skip: {
        const int affected = nextSeat(seat, 1);
        common::TableMessage notice;
        notice.type = QStringLiteral("notice");
        notice.payload[QStringLiteral("kind")] = QStringLiteral("skip");
        notice.payload[QStringLiteral("seat")] = affected;
        emit broadcastMessage(notice);
        m_currentSeat = nextSeat(seat, 2);
        break;
    }
    case SpecialEffect::None:
        m_currentSeat = nextSeat(seat, 1);
        break;
    }

    publishHand(seat);
    publishPublicState();
    emit gameStateChanged();
}

void MauMauGame::publishHand(int seat)
{
    common::TableMessage message;
    message.type = QStringLiteral("hand");
    message.seat = seat;
    message.payload[QStringLiteral("cards")] = toJson(m_hands.at(static_cast<std::size_t>(seat)));
    emit seatMessage(seat, message);
}

void MauMauGame::publishPublicState()
{
    common::TableMessage message;
    message.type = QStringLiteral("public_state");

    QJsonArray handCounts;
    for (int seat = 0; seat < m_seatCount; ++seat) {
        handCounts.append(static_cast<int>(m_hands.at(static_cast<std::size_t>(seat)).size()));
    }

    message.payload[QStringLiteral("topCard")] = toJson(topCard());
    message.payload[QStringLiteral("wishedSuit")] =
        m_wishedSuit ? QString::fromStdString(std::string(toString(*m_wishedSuit))) : QJsonValue();
    message.payload[QStringLiteral("handCounts")] = handCounts;
    message.payload[QStringLiteral("currentSeat")] = m_currentSeat;
    message.payload[QStringLiteral("drawPileCount")] = drawPileCount();
    message.payload[QStringLiteral("phase")] =
        m_winnerSeat ? QStringLiteral("round_over") : QStringLiteral("playing");
    message.payload[QStringLiteral("winnerSeat")] = m_winnerSeat ? QJsonValue(*m_winnerSeat) : QJsonValue();

    emit broadcastMessage(message);
}

void MauMauGame::runBotTurnsIfAny()
{
    while (!m_winnerSeat.has_value() && isBotSeat(m_currentSeat)) {
        const int seat = m_currentSeat;
        auto& hand = m_hands.at(static_cast<std::size_t>(seat));
        const DiscardState state{topCard(), m_wishedSuit};

        const auto playableIt = std::ranges::find_if(hand, [&state](const Card& card) { return isLegalPlay(card, state); });
        if (playableIt == hand.end()) {
            giveDrawCards(seat, 1);
            m_currentSeat = nextSeat(seat, 1);
            publishHand(seat);
            publishPublicState();
            emit gameStateChanged();
            continue;
        }

        const Card chosen = *playableIt;
        hand.erase(playableIt);

        std::optional<Suit> wish;
        if (chosen.rank == Rank::Jack) {
            // Basic, non-strategic heuristic: wish for whichever suit the bot now holds the
            // most of. Falls back to Clubs if the bot's hand is (now) empty of any suit tally.
            std::array<int, 4> tally{};
            for (const Card& card : hand) {
                ++tally.at(static_cast<std::size_t>(card.suit));
            }
            const auto bestIt = std::ranges::max_element(tally);
            wish = static_cast<Suit>(std::distance(tally.begin(), bestIt));
        }

        applyPlay(seat, chosen, wish);
    }
}

} // namespace qttutorial::maumau
