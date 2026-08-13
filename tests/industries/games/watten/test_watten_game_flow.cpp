// SPDX-License-Identifier: MIT
#include "BummerlScore.h"
#include "WattenGame.h"

#include <QSignalSpy>
#include <QTest>

using namespace qttutorial::watten;

namespace {

// Plays whatever card is currently at the front of the seat-to-move's hand
// -- since this implementation has no follow-suit obligation, any card is
// always legal, so this always succeeds while the hand has cards left.
bool playFrontCard(WattenGame& game)
{
    const int seat = game.currentTurnSeat();
    const auto& hand = game.hand(seat);
    if (hand.empty()) {
        return false;
    }
    return game.playCard(seat, hand.front());
}

} // namespace

class TestWattenGameFlow : public QObject {
    Q_OBJECT
private slots:
    void playingThroughAHandEmitsOneTrickPerFourCardsUntilDecided()
    {
        WattenGame game;
        QSignalSpy trickSpy(&game, &WattenGame::trickWon);
        QSignalSpy handSpy(&game, &WattenGame::handWon);
        QSignalSpy trumpSpy(&game, &WattenGame::trumpDecided);

        game.startNewHand();

        int cardsPlayed = 0;
        while (handSpy.isEmpty() && cardsPlayed < WattenGame::kSeatCount * WattenGame::kCardsPerHand) {
            QVERIFY(playFrontCard(game));
            ++cardsPlayed;
        }

        QCOMPARE(trumpSpy.count(), 1);
        QCOMPARE(handSpy.count(), 1);
        QVERIFY(!trickSpy.isEmpty());
        QVERIFY(trickSpy.count() <= WattenGame::kCardsPerHand);

        const Team winner = qvariant_cast<Team>(handSpy.first().at(0));
        const int winningTeamTricks = game.tricksWonBySeat(winner == Team::TeamA ? 0 : 1)
            + game.tricksWonBySeat(winner == Team::TeamA ? 2 : 3);
        QVERIFY(winningTeamTricks >= WattenGame::kTricksToWinHand);

        QCOMPARE(game.bummerlScore().score(winner), 1);
    }

    void handEndsAsSoonAsATeamReachesThreeTrickWins()
    {
        WattenGame game;
        QSignalSpy trickSpy(&game, &WattenGame::trickWon);
        QSignalSpy handSpy(&game, &WattenGame::handWon);

        game.startNewHand();
        while (handSpy.isEmpty()) {
            QVERIFY(playFrontCard(game));
        }

        // The hand must have stopped the moment a team hit 3 trick wins --
        // it cannot have run all the way to 5 tricks unless the majority
        // happened to only resolve on the last one.
        QVERIFY(trickSpy.count() <= WattenGame::kCardsPerHand);
        QVERIFY(trickSpy.count() >= WattenGame::kTricksToWinHand);
    }

    void secondHandRotatesTheDealerSeat()
    {
        WattenGame game;
        game.startNewHand();
        const int firstDealer = game.dealerSeat();

        QSignalSpy handSpy(&game, &WattenGame::handWon);
        while (handSpy.isEmpty()) {
            QVERIFY(playFrontCard(game));
        }

        game.startNewHand();
        QCOMPARE(game.dealerSeat(), (firstDealer + 1) % WattenGame::kSeatCount);
    }
};

QTEST_MAIN(TestWattenGameFlow)
#include "test_watten_game_flow.moc"
