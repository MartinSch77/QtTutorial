// SPDX-License-Identifier: MIT
#include "Deck.h"

#include <QTest>

using namespace qttutorial::games::skat;

class TestCardPoints : public QObject {
    Q_OBJECT
private slots:
    void perCardValuesMatchSchafkopfSharedValueTable()
    {
        QCOMPARE(cardPoints(Rank::Ass), 11);
        QCOMPARE(cardPoints(Rank::Ten), 10);
        QCOMPARE(cardPoints(Rank::Koenig), 4);
        QCOMPARE(cardPoints(Rank::Ober), 3);
        QCOMPARE(cardPoints(Rank::Unter), 2);
        QCOMPARE(cardPoints(Rank::Nine), 0);
        QCOMPARE(cardPoints(Rank::Eight), 0);
        QCOMPARE(cardPoints(Rank::Seven), 0);
    }

    void fullThirtyTwoCardDeckTotalsOneHundredAndTwentyPoints()
    {
        const std::vector<Card> deck = buildDeck();
        QCOMPARE(deck.size(), std::size_t{32});

        int total = 0;
        for (const Card& card : deck) {
            total += cardPoints(card.rank);
        }
        QCOMPARE(total, 120);
    }

    void dealtHandsAndSkatTogetherStillTotalOneHundredAndTwentyPoints()
    {
        std::mt19937 rng(42);
        const Deal deal = dealHand(rng);

        int total = 0;
        for (const auto& hand : deal.hands) {
            QCOMPARE(hand.size(), std::size_t{10});
            for (const Card& card : hand) {
                total += cardPoints(card.rank);
            }
        }
        QCOMPARE(deal.skat.size(), std::size_t{2});
        for (const Card& card : deal.skat) {
            total += cardPoints(card.rank);
        }
        QCOMPARE(total, 120);
    }
};

QTEST_APPLESS_MAIN(TestCardPoints)
#include "test_card_points.moc"
