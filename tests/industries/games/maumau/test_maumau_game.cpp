// SPDX-License-Identifier: MIT
#include <CardCodec.h>
#include <Deck.h>
#include <MauMauGame.h>

#include <TableMessage.h>

#include <QSignalSpy>
#include <QTest>

#include <algorithm>
#include <optional>

using namespace qttutorial::maumau;
using qttutorial::games::common::TableMessage;

namespace {

TableMessage playCardMessage(int seat, Rank rank, Suit suit, std::optional<Suit> wish = std::nullopt)
{
    TableMessage message;
    message.type = QStringLiteral("play_card");
    message.seat = seat;
    message.payload = toJson(Card{suit, rank});
    if (wish) {
        message.payload[QStringLiteral("wish")] = QString::fromStdString(std::string(toString(*wish)));
    }
    return message;
}

TableMessage drawCardMessage(int seat)
{
    TableMessage message;
    message.type = QStringLiteral("draw_card");
    message.seat = seat;
    return message;
}

// Builds a full 32-card deck whose deal order (MauMauGame deals round-robin, one card per seat
// per pass) produces exactly `hands` as the dealt hands and `discardStarter` as the first
// discard-pile card, regardless of what the (irrelevant, for these tests) draw pile ends up
// holding.
std::vector<Card> buildDeck(const std::vector<std::vector<Card>>& hands, const Card& discardStarter)
{
    std::vector<Card> deck;
    const std::size_t handSize = hands.front().size();
    for (std::size_t slot = 0; slot < handSize; ++slot) {
        for (const auto& hand : hands) {
            deck.push_back(hand.at(slot));
        }
    }
    deck.push_back(discardStarter);

    for (const Card& card : fullDeck()) {
        if (std::ranges::find(deck, card) == deck.end()) {
            deck.push_back(card);
        }
    }
    return deck;
}

QString reasonOf(const TableMessage& message)
{
    return message.payload.value(QStringLiteral("reason")).toString();
}

} // namespace

class TestMauMauGame : public QObject {
    Q_OBJECT
private slots:
    void dealsFiveCardsPerSeat();
    void sevenForcesNextPlayerToDrawTwoAndSkipsThem();
    void eightSkipsNextPlayerEntirely();
    void skipAtTwoSeatsLandsBackOnTheSamePlayer();
    void jackWishConstrainsOnlyTheImmediateNextPlay();
    void violatingAPendingWishIsRejected();
    void playingOutOfTurnIsRejected();
    void playingACardNotInHandIsRejected();
    void emptyingYourHandWinsTheRound();
};

void TestMauMauGame::dealsFiveCardsPerSeat()
{
    MauMauGame game;
    game.startNewGame(3, /*seed=*/1);

    for (int seat = 0; seat < 3; ++seat) {
        QCOMPARE(game.handOf(seat).size(), std::size_t(5));
    }
    QCOMPARE(game.drawPileCount(), 32 - 3 * 5 - 1);
    QCOMPARE(game.currentSeat(), 0);
    QVERIFY(!game.isRoundOver());
}

void TestMauMauGame::sevenForcesNextPlayerToDrawTwoAndSkipsThem()
{
    MauMauGame game;
    const std::vector<Card> hand0 = {Card{Suit::Clubs, Rank::Seven}, Card{Suit::Clubs, Rank::Nine},
                                      Card{Suit::Clubs, Rank::Ten}, Card{Suit::Clubs, Rank::Queen},
                                      Card{Suit::Clubs, Rank::King}};
    const std::vector<Card> hand1 = {Card{Suit::Spades, Rank::Nine}, Card{Suit::Spades, Rank::Ten},
                                      Card{Suit::Spades, Rank::Queen}, Card{Suit::Spades, Rank::King},
                                      Card{Suit::Spades, Rank::Ace}};
    // Discard starter's rank (Seven) matches the card seat 0 is about to play, but that is
    // irrelevant here since Seven is legal against basically anything as long as either its own
    // suit or rank matches -- pick Diamonds Seven so the rank match is what makes it legal.
    game.startNewGameWithFixedDeck(2, buildDeck({hand0, hand1}, Card{Suit::Diamonds, Rank::Seven}));

    QSignalSpy broadcastSpy(&game, &MauMauGame::broadcastMessage);
    game.handleMessage(0, playCardMessage(0, Rank::Seven, Suit::Clubs));

    QCOMPARE(game.handOf(1).size(), std::size_t(7));
    QCOMPARE(game.currentSeat(), 0); // 2 seats: skipping seat 1 lands back on seat 0
    QVERIFY(!game.isRoundOver());

    bool sawDrawTwoNotice = false;
    for (const auto& call : broadcastSpy) {
        const auto message = call.at(0).value<TableMessage>();
        if (message.type == QStringLiteral("notice")
            && message.payload.value(QStringLiteral("kind")).toString() == QStringLiteral("draw_two")) {
            QCOMPARE(message.payload.value(QStringLiteral("seat")).toInt(), 1);
            sawDrawTwoNotice = true;
        }
    }
    QVERIFY(sawDrawTwoNotice);
}

void TestMauMauGame::eightSkipsNextPlayerEntirely()
{
    MauMauGame game;
    const std::vector<Card> hand0 = {Card{Suit::Clubs, Rank::Eight}, Card{Suit::Clubs, Rank::Nine},
                                      Card{Suit::Clubs, Rank::Ten}, Card{Suit::Clubs, Rank::Queen},
                                      Card{Suit::Clubs, Rank::King}};
    const std::vector<Card> hand1 = {Card{Suit::Spades, Rank::Nine}, Card{Suit::Spades, Rank::Ten},
                                      Card{Suit::Spades, Rank::Queen}, Card{Suit::Spades, Rank::King},
                                      Card{Suit::Spades, Rank::Ace}};
    const std::vector<Card> hand2 = {Card{Suit::Hearts, Rank::Nine}, Card{Suit::Hearts, Rank::Ten},
                                      Card{Suit::Hearts, Rank::Queen}, Card{Suit::Hearts, Rank::King},
                                      Card{Suit::Hearts, Rank::Ace}};
    game.startNewGameWithFixedDeck(3, buildDeck({hand0, hand1, hand2}, Card{Suit::Diamonds, Rank::Eight}));

    game.handleMessage(0, playCardMessage(0, Rank::Eight, Suit::Clubs));

    QCOMPARE(game.handOf(1).size(), std::size_t(5)); // untouched, just skipped
    QCOMPARE(game.currentSeat(), 2);
}

void TestMauMauGame::skipAtTwoSeatsLandsBackOnTheSamePlayer()
{
    MauMauGame game;
    const std::vector<Card> hand0 = {Card{Suit::Clubs, Rank::Eight}, Card{Suit::Clubs, Rank::Nine},
                                      Card{Suit::Clubs, Rank::Ten}, Card{Suit::Clubs, Rank::Queen},
                                      Card{Suit::Clubs, Rank::King}};
    const std::vector<Card> hand1 = {Card{Suit::Spades, Rank::Nine}, Card{Suit::Spades, Rank::Ten},
                                      Card{Suit::Spades, Rank::Queen}, Card{Suit::Spades, Rank::King},
                                      Card{Suit::Spades, Rank::Ace}};
    game.startNewGameWithFixedDeck(2, buildDeck({hand0, hand1}, Card{Suit::Diamonds, Rank::Eight}));

    game.handleMessage(0, playCardMessage(0, Rank::Eight, Suit::Clubs));

    QCOMPARE(game.currentSeat(), 0);
}

void TestMauMauGame::jackWishConstrainsOnlyTheImmediateNextPlay()
{
    MauMauGame game;
    const std::vector<Card> hand0 = {Card{Suit::Clubs, Rank::Jack}, Card{Suit::Clubs, Rank::Nine},
                                      Card{Suit::Clubs, Rank::Ten}, Card{Suit::Clubs, Rank::Queen},
                                      Card{Suit::Clubs, Rank::King}};
    const std::vector<Card> hand1 = {Card{Suit::Hearts, Rank::Nine}, Card{Suit::Spades, Rank::Ten},
                                      Card{Suit::Spades, Rank::Queen}, Card{Suit::Spades, Rank::King},
                                      Card{Suit::Spades, Rank::Ace}};
    const std::vector<Card> hand2 = {Card{Suit::Diamonds, Rank::Nine}, Card{Suit::Diamonds, Rank::Ten},
                                      Card{Suit::Diamonds, Rank::Queen}, Card{Suit::Diamonds, Rank::King},
                                      Card{Suit::Diamonds, Rank::Ace}};
    game.startNewGameWithFixedDeck(3, buildDeck({hand0, hand1, hand2}, Card{Suit::Diamonds, Rank::Seven}));

    game.handleMessage(0, playCardMessage(0, Rank::Jack, Suit::Clubs, Suit::Hearts));
    QVERIFY(game.wishedSuit().has_value());
    QVERIFY(*game.wishedSuit() == Suit::Hearts);
    QCOMPARE(game.currentSeat(), 1);

    game.handleMessage(1, playCardMessage(1, Rank::Nine, Suit::Hearts));
    QVERIFY(!game.wishedSuit().has_value());
    QCOMPARE(game.currentSeat(), 2);

    // Seat 2 is judged against the new top card (Hearts Nine), not the old wish -- a Diamonds
    // Nine does not match Hearts Nine by suit, but does by rank.
    game.handleMessage(2, playCardMessage(2, Rank::Nine, Suit::Diamonds));
    QCOMPARE(game.currentSeat(), 0);
}

void TestMauMauGame::violatingAPendingWishIsRejected()
{
    MauMauGame game;
    const std::vector<Card> hand0 = {Card{Suit::Clubs, Rank::Jack}, Card{Suit::Clubs, Rank::Nine},
                                      Card{Suit::Clubs, Rank::Ten}, Card{Suit::Clubs, Rank::Queen},
                                      Card{Suit::Clubs, Rank::King}};
    const std::vector<Card> hand1 = {Card{Suit::Spades, Rank::Nine}, Card{Suit::Spades, Rank::Ten},
                                      Card{Suit::Spades, Rank::Queen}, Card{Suit::Spades, Rank::King},
                                      Card{Suit::Spades, Rank::Ace}};
    game.startNewGameWithFixedDeck(2, buildDeck({hand0, hand1}, Card{Suit::Diamonds, Rank::Seven}));
    game.handleMessage(0, playCardMessage(0, Rank::Jack, Suit::Clubs, Suit::Hearts));

    QSignalSpy seatSpy(&game, &MauMauGame::seatMessage);
    game.handleMessage(1, playCardMessage(1, Rank::Nine, Suit::Spades));

    QCOMPARE(game.currentSeat(), 1);
    QCOMPARE(game.handOf(1).size(), std::size_t(5));
    QVERIFY(seatSpy.count() > 0);
    const auto lastMessage = seatSpy.constLast().at(1).value<TableMessage>();
    QCOMPARE(lastMessage.type, QStringLiteral("action_error"));
    QCOMPARE(reasonOf(lastMessage), QStringLiteral("illegal_card"));
}

void TestMauMauGame::playingOutOfTurnIsRejected()
{
    MauMauGame game;
    game.startNewGame(2, /*seed=*/7);

    QSignalSpy seatSpy(&game, &MauMauGame::seatMessage);
    game.handleMessage(1, drawCardMessage(1));

    const auto lastMessage = seatSpy.constLast().at(1).value<TableMessage>();
    QCOMPARE(lastMessage.type, QStringLiteral("action_error"));
    QCOMPARE(reasonOf(lastMessage), QStringLiteral("not_your_turn"));
}

void TestMauMauGame::playingACardNotInHandIsRejected()
{
    MauMauGame game;
    game.startNewGame(2, /*seed=*/7);

    const Card notInHand = [&game] {
        for (const Card& candidate : fullDeck()) {
            if (std::ranges::find(game.handOf(0), candidate) == game.handOf(0).end()) {
                return candidate;
            }
        }
        return Card{Suit::Clubs, Rank::Seven};
    }();

    QSignalSpy seatSpy(&game, &MauMauGame::seatMessage);
    game.handleMessage(0, playCardMessage(0, notInHand.rank, notInHand.suit));

    const auto lastMessage = seatSpy.constLast().at(1).value<TableMessage>();
    QCOMPARE(lastMessage.type, QStringLiteral("action_error"));
    QCOMPARE(reasonOf(lastMessage), QStringLiteral("card_not_in_hand"));
}

void TestMauMauGame::emptyingYourHandWinsTheRound()
{
    MauMauGame game;
    const std::vector<Card> hand0 = {Card{Suit::Clubs, Rank::Nine}, Card{Suit::Clubs, Rank::Ten},
                                      Card{Suit::Clubs, Rank::Queen}, Card{Suit::Clubs, Rank::King},
                                      Card{Suit::Clubs, Rank::Ace}};
    // All-Clubs hand and an also-Clubs discard starter: every play in sequence matches the
    // (ever-changing) top card by suit, so nothing here needs the draw pile.
    game.startNewGameWithFixedDeck(1, buildDeck({hand0}, Card{Suit::Clubs, Rank::Seven}));

    QSignalSpy roundOverSpy(&game, &MauMauGame::roundOver);

    game.handleMessage(0, playCardMessage(0, Rank::Nine, Suit::Clubs));
    QVERIFY(!game.isRoundOver());
    game.handleMessage(0, playCardMessage(0, Rank::Ten, Suit::Clubs));
    game.handleMessage(0, playCardMessage(0, Rank::Queen, Suit::Clubs));
    game.handleMessage(0, playCardMessage(0, Rank::King, Suit::Clubs));
    QVERIFY(!game.isRoundOver());
    game.handleMessage(0, playCardMessage(0, Rank::Ace, Suit::Clubs));

    QVERIFY(game.isRoundOver());
    QCOMPARE(*game.winnerSeat(), 0);
    QCOMPARE(roundOverSpy.count(), 1);
    QCOMPARE(roundOverSpy.constLast().at(0).toInt(), 0);
}

QTEST_APPLESS_MAIN(TestMauMauGame)
#include "test_maumau_game.moc"
