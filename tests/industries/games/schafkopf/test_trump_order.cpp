// SPDX-License-Identifier: MIT
#include "TrumpOrder.h"

#include <QTest>

using namespace qttutorial::games::schafkopf;

class TestTrumpOrder : public QObject {
    Q_OBJECT
private slots:
    void allObersAndUntersAndHerzAreTrump()
    {
        QVERIFY(isTrump(Card{Suit::Eichel, Rank::Ober}));
        QVERIFY(isTrump(Card{Suit::Gras, Rank::Ober}));
        QVERIFY(isTrump(Card{Suit::Herz, Rank::Ober}));
        QVERIFY(isTrump(Card{Suit::Schellen, Rank::Ober}));
        QVERIFY(isTrump(Card{Suit::Eichel, Rank::Unter}));
        QVERIFY(isTrump(Card{Suit::Gras, Rank::Unter}));
        QVERIFY(isTrump(Card{Suit::Herz, Rank::Unter}));
        QVERIFY(isTrump(Card{Suit::Schellen, Rank::Unter}));
        for (Rank rank : {Rank::Ass, Rank::Zehn, Rank::Koenig, Rank::Neun, Rank::Acht, Rank::Sieben}) {
            QVERIFY(isTrump(Card{Suit::Herz, rank}));
        }
    }

    void plainSuitCardsAreNotTrump()
    {
        for (Suit suit : {Suit::Eichel, Suit::Gras, Suit::Schellen}) {
            for (Rank rank : {Rank::Ass, Rank::Zehn, Rank::Koenig, Rank::Neun, Rank::Acht, Rank::Sieben}) {
                QVERIFY(!isTrump(Card{suit, rank}));
            }
        }
    }

    void obersOutrankUntersOutrankHerzRunAmongTrump()
    {
        const auto s = [](const Card& c) { return *trumpStrength(c); };
        QVERIFY(s(Card{Suit::Eichel, Rank::Ober}) > s(Card{Suit::Gras, Rank::Ober}));
        QVERIFY(s(Card{Suit::Gras, Rank::Ober}) > s(Card{Suit::Herz, Rank::Ober}));
        QVERIFY(s(Card{Suit::Herz, Rank::Ober}) > s(Card{Suit::Schellen, Rank::Ober}));
        QVERIFY(s(Card{Suit::Schellen, Rank::Ober}) > s(Card{Suit::Eichel, Rank::Unter}));
        QVERIFY(s(Card{Suit::Eichel, Rank::Unter}) > s(Card{Suit::Gras, Rank::Unter}));
        QVERIFY(s(Card{Suit::Schellen, Rank::Unter}) > s(Card{Suit::Herz, Rank::Ass}));
        QVERIFY(s(Card{Suit::Herz, Rank::Ass}) > s(Card{Suit::Herz, Rank::Zehn}));
        QVERIFY(s(Card{Suit::Herz, Rank::Zehn}) > s(Card{Suit::Herz, Rank::Koenig}));
        QVERIFY(s(Card{Suit::Herz, Rank::Koenig}) > s(Card{Suit::Herz, Rank::Neun}));
        QVERIFY(s(Card{Suit::Herz, Rank::Neun}) > s(Card{Suit::Herz, Rank::Acht}));
        QVERIFY(s(Card{Suit::Herz, Rank::Acht}) > s(Card{Suit::Herz, Rank::Sieben}));
    }

    void plainSuitRankOrderWithinASuit()
    {
        const auto s = [](const Card& c) { return *plainStrength(c); };
        QVERIFY(s(Card{Suit::Eichel, Rank::Ass}) > s(Card{Suit::Eichel, Rank::Zehn}));
        QVERIFY(s(Card{Suit::Eichel, Rank::Zehn}) > s(Card{Suit::Eichel, Rank::Koenig}));
        QVERIFY(s(Card{Suit::Eichel, Rank::Koenig}) > s(Card{Suit::Eichel, Rank::Neun}));
        QVERIFY(s(Card{Suit::Eichel, Rank::Neun}) > s(Card{Suit::Eichel, Rank::Acht}));
        QVERIFY(s(Card{Suit::Eichel, Rank::Acht}) > s(Card{Suit::Eichel, Rank::Sieben}));
    }

    void trumpCardsHaveNoPlainStrengthAndViceVersa()
    {
        QVERIFY(!plainStrength(Card{Suit::Herz, Rank::Ass}).has_value());
        QVERIFY(!plainStrength(Card{Suit::Eichel, Rank::Ober}).has_value());
        QVERIFY(!trumpStrength(Card{Suit::Eichel, Rank::Ass}).has_value());
    }
};

QTEST_APPLESS_MAIN(TestTrumpOrder)
#include "test_trump_order.moc"
