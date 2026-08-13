// SPDX-License-Identifier: MIT
#include "TrumpOrder.h"

#include <QTest>

using namespace qttutorial::games::schafkopf;

class TestTrickWinner : public QObject {
    Q_OBJECT
private slots:
    void plainSuitTrickHighestOfLedSuitWins()
    {
        // Eichel led; no trump played. Eichel-Ass should win over Eichel-Koenig
        // and Eichel-Neun, and the off-suit Gras-Ass (which could not have
        // legally been played here, but the resolver must still ignore it).
        const std::vector<Card> trick{
            Card{Suit::Eichel, Rank::Koenig},
            Card{Suit::Eichel, Rank::Ass},
            Card{Suit::Gras, Rank::Ass},
            Card{Suit::Eichel, Rank::Neun},
        };
        QCOMPARE(trickWinnerIndex(trick), std::size_t{1});
    }

    void trumpBeatsLedPlainSuitEvenIfLower()
    {
        // Schellen led; seat 2 trumps in with the weakest trump (Herz-Sieben),
        // which still beats every plain Schellen card.
        const std::vector<Card> trick{
            Card{Suit::Schellen, Rank::Ass},
            Card{Suit::Schellen, Rank::Zehn},
            Card{Suit::Herz, Rank::Sieben},
            Card{Suit::Schellen, Rank::Koenig},
        };
        QCOMPARE(trickWinnerIndex(trick), std::size_t{2});
    }

    void allTrumpTrickHighestTrumpWins()
    {
        const std::vector<Card> trick{
            Card{Suit::Herz, Rank::Unter},
            Card{Suit::Eichel, Rank::Ober},
            Card{Suit::Herz, Rank::Ass},
            Card{Suit::Schellen, Rank::Ober},
        };
        QCOMPARE(trickWinnerIndex(trick), std::size_t{1});
    }

    void leaderWinsIfNobodyBeatsThem()
    {
        const std::vector<Card> trick{
            Card{Suit::Gras, Rank::Ass},
            Card{Suit::Gras, Rank::Koenig},
            Card{Suit::Gras, Rank::Neun},
            Card{Suit::Gras, Rank::Acht},
        };
        QCOMPARE(trickWinnerIndex(trick), std::size_t{0});
    }
};

QTEST_APPLESS_MAIN(TestTrickWinner)
#include "test_trick_winner.moc"
