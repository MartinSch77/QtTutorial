// SPDX-License-Identifier: MIT
#include "Trump.h"

#include <QTest>

using namespace qttutorial::games::skat;

class TestTrumpOrder : public QObject {
    Q_OBJECT
private slots:
    void suitGame_allFourUntersAreTrump()
    {
        QVERIFY(isTrump({Suit::Eichel, Rank::Unter}, GameType::Suit, Suit::Herz));
        QVERIFY(isTrump({Suit::Gras, Rank::Unter}, GameType::Suit, Suit::Herz));
        QVERIFY(isTrump({Suit::Herz, Rank::Unter}, GameType::Suit, Suit::Herz));
        QVERIFY(isTrump({Suit::Schellen, Rank::Unter}, GameType::Suit, Suit::Herz));
    }

    void suitGame_allFourObersAreTrump()
    {
        // This tutorial's deliberate simplification: every Ober is trump in
        // a Suit game, not just the trump suit's own -- see README
        // "Simplifications".
        QVERIFY(isTrump({Suit::Eichel, Rank::Ober}, GameType::Suit, Suit::Herz));
        QVERIFY(isTrump({Suit::Gras, Rank::Ober}, GameType::Suit, Suit::Herz));
        QVERIFY(isTrump({Suit::Herz, Rank::Ober}, GameType::Suit, Suit::Herz));
        QVERIFY(isTrump({Suit::Schellen, Rank::Ober}, GameType::Suit, Suit::Herz));
    }

    void suitGame_onlyTrumpSuitsRemainingCardsAreTrump()
    {
        QVERIFY(isTrump({Suit::Herz, Rank::Ass}, GameType::Suit, Suit::Herz));
        QVERIFY(isTrump({Suit::Herz, Rank::Seven}, GameType::Suit, Suit::Herz));
        QVERIFY(!isTrump({Suit::Eichel, Rank::Ass}, GameType::Suit, Suit::Herz));
        QVERIFY(!isTrump({Suit::Gras, Rank::Ten}, GameType::Suit, Suit::Herz));
    }

    void suitGame_trumpOrderIsUntersThenObersThenTrumpSuitRanks()
    {
        // Eichel > Gras > Herz > Schellen within Unters, and within Obers.
        QVERIFY(cardStrength({Suit::Eichel, Rank::Unter}, GameType::Suit, Suit::Herz)
                > cardStrength({Suit::Schellen, Rank::Unter}, GameType::Suit, Suit::Herz));
        QVERIFY(cardStrength({Suit::Schellen, Rank::Unter}, GameType::Suit, Suit::Herz)
                > cardStrength({Suit::Eichel, Rank::Ober}, GameType::Suit, Suit::Herz));
        QVERIFY(cardStrength({Suit::Schellen, Rank::Ober}, GameType::Suit, Suit::Herz)
                > cardStrength({Suit::Herz, Rank::Ass}, GameType::Suit, Suit::Herz));
        QVERIFY(cardStrength({Suit::Herz, Rank::Ass}, GameType::Suit, Suit::Herz)
                > cardStrength({Suit::Herz, Rank::Ten}, GameType::Suit, Suit::Herz));
        QVERIFY(cardStrength({Suit::Herz, Rank::Ten}, GameType::Suit, Suit::Herz)
                > cardStrength({Suit::Herz, Rank::Koenig}, GameType::Suit, Suit::Herz));
    }

    void grandGame_onlyUntersAreTrump()
    {
        QVERIFY(isTrump({Suit::Eichel, Rank::Unter}, GameType::Grand, Suit::Herz));
        QVERIFY(!isTrump({Suit::Herz, Rank::Ober}, GameType::Grand, Suit::Herz));
        QVERIFY(!isTrump({Suit::Herz, Rank::Ass}, GameType::Grand, Suit::Herz));
        // trumpSuit is meaningless for Grand -- nothing outside the Unters
        // should ever come back trump, regardless of what is passed here.
        QVERIFY(!isTrump({Suit::Gras, Rank::Ober}, GameType::Grand, Suit::Gras));
    }

    void grandGame_obersAreOrdinaryPlainSuitCards()
    {
        // The bug this is guarding against: treating a non-trump suit's
        // Ober as if it partially retained trump status. In Grand, Ober
        // ranks between König and 9 within its own plain suit.
        QVERIFY(cardStrength({Suit::Gras, Rank::Koenig}, GameType::Grand, Suit::Herz)
                > cardStrength({Suit::Gras, Rank::Ober}, GameType::Grand, Suit::Herz));
        QVERIFY(cardStrength({Suit::Gras, Rank::Ober}, GameType::Grand, Suit::Herz)
                > cardStrength({Suit::Gras, Rank::Nine}, GameType::Grand, Suit::Herz));
    }

    void trickWinner_suitGame_trumpBeatsLedPlainSuit()
    {
        const std::vector<Card> played{
            {Suit::Gras, Rank::Ass},    // led, plain, strong
            {Suit::Herz, Rank::Seven},  // trump (trumpSuit == Herz), weak trump but still wins
            {Suit::Gras, Rank::Koenig},
        };
        QCOMPARE(trickWinner(played, GameType::Suit, Suit::Herz), std::size_t{1});
    }

    void trickWinner_suitGame_highestOfLedSuitWinsWhenNoTrumpPlayed()
    {
        const std::vector<Card> played{
            {Suit::Gras, Rank::Ass},
            {Suit::Gras, Rank::Ten},
            {Suit::Eichel, Rank::Ass}, // different, non-trump suit: irrelevant
        };
        QCOMPARE(trickWinner(played, GameType::Suit, Suit::Herz), std::size_t{0});
    }

    void trickWinner_grandGame_untersOutrankEverythingElse()
    {
        const std::vector<Card> played{
            {Suit::Gras, Rank::Ass},
            {Suit::Schellen, Rank::Unter}, // lowest-ranked Unter, still trump in Grand
            {Suit::Gras, Rank::Koenig},
        };
        QCOMPARE(trickWinner(played, GameType::Grand, Suit::Herz), std::size_t{1});
    }

    void trickWinner_grandGame_obersDoNotActAsTrump()
    {
        const std::vector<Card> played{
            {Suit::Gras, Rank::Ober}, // led; must NOT behave as trump in Grand
            {Suit::Gras, Rank::Ass},
            {Suit::Gras, Rank::Koenig},
        };
        QCOMPARE(trickWinner(played, GameType::Grand, Suit::Herz), std::size_t{1});
    }
};

QTEST_APPLESS_MAIN(TestTrumpOrder)
#include "test_skat_trump_order.moc"
