// SPDX-License-Identifier: MIT
#include "Deck.h"
#include "SkatGame.h"

#include <QJsonArray>
#include <QSignalSpy>
#include <QTest>

#include <algorithm>

using namespace qttutorial::games::skat;
using qttutorial::games::common::TableMessage;

namespace {
// Dealer starts at kSeatCount - 1, so the first beginNewHand() rotates the
// dealer to seat 0 and makes {1, 2, 3} the three active, all-human seats --
// deterministic and free of any bot auto-play interfering with the test.
void setUpAllHumanTable(SkatGame& game)
{
    for (int seat = 0; seat < SkatGame::kSeatCount; ++seat) {
        game.setSeatHuman(seat, true);
    }
    game.beginNewHand();
}
} // namespace

class TestSkatPickupDiscard : public QObject {
    Q_OBJECT
private slots:
    void dealtHandHasTenCards()
    {
        SkatGame game;
        setUpAllHumanTable(game);

        for (int seat : game.activeSeats()) {
            QCOMPARE(game.handOf(seat).size(), std::size_t{10});
        }
    }

    void declarerHandGrowsToTwelveAfterPickup()
    {
        SkatGame game;
        setUpAllHumanTable(game);

        const int forehand = game.activeSeats()[0];
        const int middlehand = game.activeSeats()[1];
        const int rearhand = game.activeSeats()[2];

        game.applyMessage(forehand, TableMessage{QStringLiteral("bid"), forehand, {{QStringLiteral("amount"), 18}}});
        game.applyMessage(middlehand, TableMessage{QStringLiteral("pass"), middlehand, {}});
        game.applyMessage(rearhand, TableMessage{QStringLiteral("pass"), rearhand, {}});

        QCOMPARE(game.phase(), SkatGame::Phase::Discarding);
        QCOMPARE(game.declarerSeat(), forehand);
        QCOMPARE(game.handOf(forehand).size(), std::size_t{12});
    }

    void discardBringsDeclarerHandBackToTenAndBecomesTheNewSkat()
    {
        SkatGame game;
        setUpAllHumanTable(game);

        const int forehand = game.activeSeats()[0];
        const int middlehand = game.activeSeats()[1];
        const int rearhand = game.activeSeats()[2];

        game.applyMessage(forehand, TableMessage{QStringLiteral("bid"), forehand, {{QStringLiteral("amount"), 18}}});
        game.applyMessage(middlehand, TableMessage{QStringLiteral("pass"), middlehand, {}});
        game.applyMessage(rearhand, TableMessage{QStringLiteral("pass"), rearhand, {}});

        const std::vector<Card> twelveCards = game.handOf(forehand);
        QCOMPARE(twelveCards.size(), std::size_t{12});

        const Card discard1 = twelveCards[0];
        const Card discard2 = twelveCards[1];
        game.applyMessage(forehand, TableMessage{QStringLiteral("discard"), forehand,
                                                  {{QStringLiteral("cards"), QJsonArray{cardCode(discard1), cardCode(discard2)}}}});

        QCOMPARE(game.phase(), SkatGame::Phase::Announcing);
        QCOMPARE(game.handOf(forehand).size(), std::size_t{10});

        const std::vector<Card>& newSkat = game.skat();
        QCOMPARE(newSkat.size(), std::size_t{2});
        QVERIFY((newSkat[0] == discard1 && newSkat[1] == discard2) || (newSkat[0] == discard2 && newSkat[1] == discard1));
    }

    void rejectsDiscardingACardNotInHand()
    {
        SkatGame game;
        setUpAllHumanTable(game);

        const int forehand = game.activeSeats()[0];
        const int middlehand = game.activeSeats()[1];
        const int rearhand = game.activeSeats()[2];

        game.applyMessage(forehand, TableMessage{QStringLiteral("bid"), forehand, {{QStringLiteral("amount"), 18}}});
        game.applyMessage(middlehand, TableMessage{QStringLiteral("pass"), middlehand, {}});
        game.applyMessage(rearhand, TableMessage{QStringLiteral("pass"), rearhand, {}});

        QSignalSpy rejectedSpy(&game, &SkatGame::actionRejected);
        const std::vector<Card> hand = game.handOf(forehand);
        const auto deck = buildDeck();
        const auto notInHandIt =
            std::find_if(deck.begin(), deck.end(), [&hand](const Card& card) { return std::find(hand.begin(), hand.end(), card) == hand.end(); });
        QVERIFY(notInHandIt != deck.end());

        game.applyMessage(forehand, TableMessage{QStringLiteral("discard"), forehand,
                                                  {{QStringLiteral("cards"), QJsonArray{cardCode(hand[0]), cardCode(*notInHandIt)}}}});

        QCOMPARE(rejectedSpy.count(), 1);
        QCOMPARE(game.handOf(forehand).size(), std::size_t{12});
    }
};

QTEST_APPLESS_MAIN(TestSkatPickupDiscard)
#include "test_skat_pickup_discard.moc"
