// SPDX-License-Identifier: MIT
#include "SkatGame.h"

#include <QSignalSpy>
#include <QTest>

using namespace qttutorial::games::skat;

// All three active seats are left non-human, so beginNewHand() plays an
// entire hand end-to-end via the basic Bot (bidding, discard, announcement,
// and all 10 tricks) with no test-side scripting needed. That lets this
// test exercise the real win/lose threshold across many different deals
// instead of just asserting on a hand-picked one.
class TestDeclarerResult : public QObject {
    Q_OBJECT
private slots:
    void declarerAndDefenderPointsAlwaysSumToOneHundredAndTwenty()
    {
        for (unsigned seed = 0; seed < 25; ++seed) {
            SkatGame game;
            game.seedRngForTests(seed);
            QSignalSpy handCompleted(&game, &SkatGame::handCompleted);

            game.beginNewHand();

            QCOMPARE(handCompleted.count(), 1);
            QCOMPARE(game.phase(), SkatGame::Phase::HandComplete);
            QCOMPARE(game.declarerPoints() + game.defenderPoints(), 120);
        }
    }

    void declarerWinsIfAndOnlyIfTheyReachSixtyOnePoints()
    {
        bool sawAWin = false;
        bool sawALoss = false;
        for (unsigned seed = 0; seed < 40; ++seed) {
            SkatGame game;
            game.seedRngForTests(seed);
            QSignalSpy handCompleted(&game, &SkatGame::handCompleted);

            game.beginNewHand();

            QCOMPARE(handCompleted.count(), 1);
            const QList<QVariant> args = handCompleted.takeFirst();
            const int declarerPoints = args.at(1).toInt();
            const bool reportedWin = args.at(3).toBool();

            QCOMPARE(reportedWin, declarerPoints >= 61);
            QCOMPARE(game.declarerWon(), reportedWin);
            sawAWin |= reportedWin;
            sawALoss |= !reportedWin;
        }
        // Across 40 varied deals both outcomes should occur at least once --
        // if not, the threshold check itself is almost certainly broken
        // (e.g. always true, or points never being tallied).
        QVERIFY(sawAWin);
        QVERIFY(sawALoss);
    }
};

QTEST_APPLESS_MAIN(TestDeclarerResult)
#include "test_declarer_result.moc"
