// SPDX-License-Identifier: MIT
#include "Card.h"
#include "TrumpRanking.h"

#include <QTest>

using namespace qttutorial::watten;

class TestTrumpRanking : public QObject {
    Q_OBJECT
private slots:
    void ranksOrderCorrectly()
    {
        QVERIFY(rankValue(Rank::Sau) > rankValue(Rank::Koenig));
        QVERIFY(rankValue(Rank::Koenig) > rankValue(Rank::Ober));
        QVERIFY(rankValue(Rank::Ober) > rankValue(Rank::Unter));
        QVERIFY(rankValue(Rank::Unter) > rankValue(Rank::Zehn));
        QVERIFY(rankValue(Rank::Zehn) > rankValue(Rank::Neun));
        QVERIFY(rankValue(Rank::Neun) > rankValue(Rank::Acht));
        QVERIFY(rankValue(Rank::Acht) > rankValue(Rank::Sieben));
    }

    void winnerIsHighestOfLedSuitWhenNoTrumpPlayed()
    {
        // Led suit Gras, trump is Eichel but no Eichel is actually played.
        const std::vector<PlayedCard> trick{
            {0, Card{Suit::Gras, Rank::Zehn}},
            {1, Card{Suit::Herz, Rank::Ober}},
            {2, Card{Suit::Gras, Rank::Sau}},
            {3, Card{Suit::Schellen, Rank::Koenig}},
        };
        QCOMPARE(trickWinnerSeat(trick, Suit::Eichel), 2);
    }

    void anyTrumpBeatsAnyNonTrumpEvenIfPlayedByFollower()
    {
        // Led suit Gras with a high card, but seat 1 (not the leader) plays
        // a low trump (Herz) and still wins the trick outright.
        const std::vector<PlayedCard> trick{
            {0, Card{Suit::Gras, Rank::Sau}},
            {1, Card{Suit::Herz, Rank::Acht}},
            {2, Card{Suit::Gras, Rank::Ober}},
            {3, Card{Suit::Schellen, Rank::Sieben}},
        };
        QCOMPARE(trickWinnerSeat(trick, Suit::Herz), 1);
    }

    void highestTrumpWinsWhenMultipleTrumpsPlayed()
    {
        const std::vector<PlayedCard> trick{
            {0, Card{Suit::Gras, Rank::Zehn}},
            {1, Card{Suit::Herz, Rank::Unter}},
            {2, Card{Suit::Herz, Rank::Sau}},
            {3, Card{Suit::Gras, Rank::Neun}},
        };
        QCOMPARE(trickWinnerSeat(trick, Suit::Herz), 2);
    }

    void cardOfNeitherLedSuitNorTrumpCanNeverWin()
    {
        const std::vector<PlayedCard> trick{
            {0, Card{Suit::Gras, Rank::Sieben}},
            {1, Card{Suit::Schellen, Rank::Sau}},
            {2, Card{Suit::Gras, Rank::Acht}},
            {3, Card{Suit::Eichel, Rank::Sau}},
        };
        QCOMPARE(trickWinnerSeat(trick, Suit::Herz), 2);
    }
};

QTEST_MAIN(TestTrumpRanking)
#include "test_trump_ranking.moc"
