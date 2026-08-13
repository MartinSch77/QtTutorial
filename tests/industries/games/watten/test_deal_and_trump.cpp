// SPDX-License-Identifier: MIT
#include "Deck.h"
#include "WattenGame.h"

#include <QTest>

#include <algorithm>
#include <set>
#include <utility>

using namespace qttutorial::watten;

class TestDealAndTrump : public QObject {
    Q_OBJECT
private slots:
    void dealsFiveDistinctCardsPerSeat()
    {
        WattenGame game;
        game.startNewHand();

        std::set<std::pair<int, int>> seenCards;
        for (int seat = 0; seat < WattenGame::kSeatCount; ++seat) {
            const auto& hand = game.hand(seat);
            QCOMPARE(static_cast<int>(hand.size()), WattenGame::kCardsPerHand);
            for (const Card& card : hand) {
                const auto key = std::make_pair(static_cast<int>(card.suit), static_cast<int>(card.rank));
                QVERIFY2(seenCards.insert(key).second, "the same card was dealt twice across seats");
            }
        }
        QCOMPARE(static_cast<int>(seenCards.size()), WattenGame::kSeatCount * WattenGame::kCardsPerHand);
    }

    void openingLeadFixesTrumpForTheWholeHand()
    {
        WattenGame game;
        game.startNewHand();

        const int leader = game.currentTurnSeat();
        QCOMPARE(leader, game.dealerSeat());
        QVERIFY(!game.trumpSuit().has_value());

        const Card leadCard = game.hand(leader).front();
        QVERIFY(game.playCard(leader, leadCard));
        QVERIFY(game.trumpSuit().has_value());
        QCOMPARE(*game.trumpSuit(), leadCard.suit);

        const int secondSeat = game.currentTurnSeat();
        Card secondCard = game.hand(secondSeat).front();
        // Deliberately try to play a card of a different suit than trump, to
        // confirm trump does not change after the opening lead. If the hand
        // happens to only contain trump cards, that is still consistent with
        // trump staying fixed, so the assertion below holds either way.
        for (const Card& candidate : game.hand(secondSeat)) {
            if (candidate.suit != leadCard.suit) {
                secondCard = candidate;
                break;
            }
        }
        QVERIFY(game.playCard(secondSeat, secondCard));
        QCOMPARE(*game.trumpSuit(), leadCard.suit);
    }

    void cannotPlayOutOfTurnOrACardNotInHand()
    {
        WattenGame game;
        game.startNewHand();

        const int leader = game.currentTurnSeat();
        const int notLeader = (leader + 1) % WattenGame::kSeatCount;

        QVERIFY(!game.canPlay(notLeader, game.hand(notLeader).front()));

        const auto& leaderHand = game.hand(leader);
        const auto deck = fullDeck();
        const auto it = std::ranges::find_if(
            deck, [&](const Card& candidate) { return std::ranges::find(leaderHand, candidate) == leaderHand.end(); });
        QVERIFY(it != deck.end());
        QVERIFY(!game.canPlay(leader, *it));
    }
};

QTEST_MAIN(TestDealAndTrump)
#include "test_deal_and_trump.moc"
