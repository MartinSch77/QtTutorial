// SPDX-License-Identifier: MIT
#include "RufspielRules.h"

#include <QTest>

using namespace qttutorial::games::schafkopf;

class TestRufspielRulesPlay : public QObject {
    Q_OBJECT
private slots:
    void anyCardIsLegalWhenLeading()
    {
        const std::vector<Card> hand{
            Card{Suit::Eichel, Rank::Ass}, Card{Suit::Herz, Rank::Ober},
        };
        QVERIFY(isLegalPlay(hand, {}, Card{Suit::Eichel, Rank::Ass}));
        QVERIFY(isLegalPlay(hand, {}, Card{Suit::Herz, Rank::Ober}));
    }

    void mustFollowLedPlainSuitIfHoldingIt()
    {
        const std::vector<Card> hand{
            Card{Suit::Eichel, Rank::Ass}, Card{Suit::Eichel, Rank::Koenig},
            Card{Suit::Herz, Rank::Ober},
        };
        const std::vector<Card> trick{Card{Suit::Eichel, Rank::Sieben}};

        QVERIFY(isLegalPlay(hand, trick, Card{Suit::Eichel, Rank::Ass}));
        QVERIFY(isLegalPlay(hand, trick, Card{Suit::Eichel, Rank::Koenig}));
        QVERIFY(!isLegalPlay(hand, trick, Card{Suit::Herz, Rank::Ober}));

        const std::vector<Card> legal = legalPlays(hand, trick);
        QCOMPARE(legal.size(), std::size_t{2});
    }

    void mayPlayAnythingIfLackingLedPlainSuit()
    {
        const std::vector<Card> hand{
            Card{Suit::Gras, Rank::Ass}, Card{Suit::Herz, Rank::Unter},
        };
        const std::vector<Card> trick{Card{Suit::Eichel, Rank::Sieben}};

        QVERIFY(isLegalPlay(hand, trick, Card{Suit::Gras, Rank::Ass}));
        QVERIFY(isLegalPlay(hand, trick, Card{Suit::Herz, Rank::Unter}));
    }

    void mustFollowTrumpIfTrumpWasLedAndHandHoldsTrump()
    {
        const std::vector<Card> hand{
            Card{Suit::Herz, Rank::Sieben}, Card{Suit::Gras, Rank::Ass},
        };
        const std::vector<Card> trick{Card{Suit::Eichel, Rank::Ober}}; // trump led

        QVERIFY(isLegalPlay(hand, trick, Card{Suit::Herz, Rank::Sieben}));
        QVERIFY(!isLegalPlay(hand, trick, Card{Suit::Gras, Rank::Ass}));
    }

    void mayPlayAnythingIfTrumpLedButHandHoldsNoTrump()
    {
        const std::vector<Card> hand{
            Card{Suit::Gras, Rank::Ass}, Card{Suit::Schellen, Rank::Sieben},
        };
        const std::vector<Card> trick{Card{Suit::Eichel, Rank::Ober}}; // trump led

        QVERIFY(isLegalPlay(hand, trick, Card{Suit::Gras, Rank::Ass}));
        QVERIFY(isLegalPlay(hand, trick, Card{Suit::Schellen, Rank::Sieben}));
    }
};

QTEST_APPLESS_MAIN(TestRufspielRulesPlay)
#include "test_rufspiel_rules_play.moc"
