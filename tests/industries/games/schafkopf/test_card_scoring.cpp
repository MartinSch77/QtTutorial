// SPDX-License-Identifier: MIT
#include "Card.h"

#include <QTest>

#include <algorithm>

using namespace qttutorial::games::schafkopf;

class TestCardScoring : public QObject {
    Q_OBJECT
private slots:
    void individualCardPointValues()
    {
        QCOMPARE(cardPoints(Card{Suit::Eichel, Rank::Ass}), 11);
        QCOMPARE(cardPoints(Card{Suit::Herz, Rank::Zehn}), 10);
        QCOMPARE(cardPoints(Card{Suit::Gras, Rank::Koenig}), 4);
        QCOMPARE(cardPoints(Card{Suit::Schellen, Rank::Ober}), 3);
        QCOMPARE(cardPoints(Card{Suit::Eichel, Rank::Unter}), 2);
        QCOMPARE(cardPoints(Card{Suit::Herz, Rank::Neun}), 0);
        QCOMPARE(cardPoints(Card{Suit::Gras, Rank::Acht}), 0);
        QCOMPARE(cardPoints(Card{Suit::Schellen, Rank::Sieben}), 0);
    }

    void fullDeckHasThirtyTwoUniqueCards()
    {
        const std::vector<Card> deck = fullDeck();
        QCOMPARE(deck.size(), std::size_t{32});
        for (std::size_t i = 0; i < deck.size(); ++i) {
            for (std::size_t j = i + 1; j < deck.size(); ++j) {
                QVERIFY(!(deck[i] == deck[j]));
            }
        }
    }

    void fullDeckPointsSumToOneHundredTwenty()
    {
        int total = 0;
        for (const Card& card : fullDeck()) {
            total += cardPoints(card);
        }
        QCOMPARE(total, 120);
    }

    void shufflingPreservesTheDeckAsAMultiset()
    {
        std::mt19937 rng(42);
        std::vector<Card> shuffled = shuffledDeck(rng);
        std::vector<Card> sortedShuffled = shuffled;
        std::vector<Card> sortedOriginal = fullDeck();
        std::ranges::sort(sortedShuffled);
        std::ranges::sort(sortedOriginal);
        QCOMPARE(shuffled.size(), std::size_t{32});
        QVERIFY(sortedShuffled == sortedOriginal);
    }
};

QTEST_APPLESS_MAIN(TestCardScoring)
#include "test_card_scoring.moc"
