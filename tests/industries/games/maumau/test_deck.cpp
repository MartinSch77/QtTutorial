// SPDX-License-Identifier: MIT
#include <Deck.h>

#include <QTest>

#include <algorithm>
#include <array>

using namespace qttutorial::maumau;

class TestDeck : public QObject {
    Q_OBJECT
private slots:
    void hasThirtyTwoCards();
    void hasFourSuitsOfEightRanksEach();
    void shuffleIsAPermutation();
};

void TestDeck::hasThirtyTwoCards()
{
    QCOMPARE(fullDeck().size(), std::size_t(32));
}

void TestDeck::hasFourSuitsOfEightRanksEach()
{
    const std::vector<Card> deck = fullDeck();

    std::array<int, 4> suitCounts{};
    std::array<int, 8> rankCounts{};
    for (const Card& card : deck) {
        ++suitCounts.at(static_cast<std::size_t>(card.suit));
        ++rankCounts.at(static_cast<std::size_t>(card.rank));
    }

    for (int count : suitCounts) {
        QCOMPARE(count, 8);
    }
    for (int count : rankCounts) {
        QCOMPARE(count, 4);
    }

    for (int i = 0; i < static_cast<int>(deck.size()); ++i) {
        for (int j = i + 1; j < static_cast<int>(deck.size()); ++j) {
            QVERIFY(!(deck[static_cast<std::size_t>(i)] == deck[static_cast<std::size_t>(j)]));
        }
    }
}

void TestDeck::shuffleIsAPermutation()
{
    std::vector<Card> deck = fullDeck();
    std::vector<Card> shuffled = deck;
    std::mt19937 rng(42);
    shuffle(shuffled, rng);

    QCOMPARE(shuffled.size(), deck.size());
    std::ranges::sort(deck);
    std::vector<Card> sortedShuffled = shuffled;
    std::ranges::sort(sortedShuffled);
    QCOMPARE(sortedShuffled, deck);
}

QTEST_APPLESS_MAIN(TestDeck)
#include "test_deck.moc"
