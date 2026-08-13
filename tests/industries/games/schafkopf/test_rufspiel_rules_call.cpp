// SPDX-License-Identifier: MIT
#include "RufspielRules.h"

#include <QTest>

#include <algorithm>

using namespace qttutorial::games::schafkopf;

class TestRufspielRulesCall : public QObject {
    Q_OBJECT
private slots:
    void herzCanNeverBeCalledBecauseItIsTrump()
    {
        const std::vector<Card> hand{
            Card{Suit::Herz, Rank::Sieben}, Card{Suit::Eichel, Rank::Sieben},
            Card{Suit::Gras, Rank::Sieben}, Card{Suit::Schellen, Rank::Sieben},
        };
        QVERIFY(!isLegalCall(hand, Suit::Herz));
    }

    void cannotCallASuitWhoseAceIsAlreadyInHand()
    {
        const std::vector<Card> hand{
            Card{Suit::Eichel, Rank::Ass}, Card{Suit::Eichel, Rank::Koenig},
            Card{Suit::Gras, Rank::Ass}, Card{Suit::Gras, Rank::Koenig},
        };
        QVERIFY(!isLegalCall(hand, Suit::Eichel));
        QVERIFY(!isLegalCall(hand, Suit::Gras));
    }

    void cannotCallASuitWithNoOtherCardOfThatSuit()
    {
        // No Schellen card at all in hand -- "Ass muss angespielt werden
        // koennen" forbids calling a suit the caller couldn't ever lead into.
        const std::vector<Card> hand{
            Card{Suit::Eichel, Rank::Koenig}, Card{Suit::Gras, Rank::Koenig},
            Card{Suit::Herz, Rank::Ober}, Card{Suit::Herz, Rank::Unter},
        };
        QVERIFY(!isLegalCall(hand, Suit::Schellen));
    }

    void obersAndUntersOfThatSuitDoNotCountAsTheExtraPlainCard()
    {
        // Only Eichel-Ober (trump) besides not having the ace -- still no
        // legal Eichel call, since Ober doesn't count as a plain Eichel card.
        const std::vector<Card> hand{
            Card{Suit::Eichel, Rank::Ober}, Card{Suit::Gras, Rank::Koenig},
            Card{Suit::Herz, Rank::Ass}, Card{Suit::Schellen, Rank::Koenig},
            Card{Suit::Schellen, Rank::Sieben},
        };
        QVERIFY(!isLegalCall(hand, Suit::Eichel));
    }

    void legalCallWhenHoldingAPlainCardOfAnUncalledAceSuit()
    {
        const std::vector<Card> hand{
            Card{Suit::Eichel, Rank::Koenig}, Card{Suit::Gras, Rank::Sieben},
            Card{Suit::Herz, Rank::Ober}, Card{Suit::Schellen, Rank::Unter},
        };
        QVERIFY(isLegalCall(hand, Suit::Eichel));
        QVERIFY(isLegalCall(hand, Suit::Gras));
        QVERIFY(!isLegalCall(hand, Suit::Schellen));

        const std::vector<Suit> options = legalCallOptions(hand);
        QCOMPARE(options.size(), std::size_t{2});
        QVERIFY(std::ranges::find(options, Suit::Eichel) != options.end());
        QVERIFY(std::ranges::find(options, Suit::Gras) != options.end());
    }
};

QTEST_APPLESS_MAIN(TestRufspielRulesCall)
#include "test_rufspiel_rules_call.moc"
