// SPDX-License-Identifier: MIT
#include "BummerlScore.h"

#include <QTest>

using namespace qttutorial::watten;

class TestBummerlScore : public QObject {
    Q_OBJECT
private slots:
    void seatsMapToTeamsAcrossTheTable()
    {
        QCOMPARE(teamForSeat(0), Team::TeamA);
        QCOMPARE(teamForSeat(2), Team::TeamA);
        QCOMPARE(teamForSeat(1), Team::TeamB);
        QCOMPARE(teamForSeat(3), Team::TeamB);
    }

    void accumulatesPointsPerTeamIndependently()
    {
        BummerlScore score;
        score.addHandWin(Team::TeamA);
        score.addHandWin(Team::TeamA);
        score.addHandWin(Team::TeamB);

        QCOMPARE(score.score(Team::TeamA), 2);
        QCOMPARE(score.score(Team::TeamB), 1);
        QVERIFY(!score.isWon());
        QVERIFY(!score.winner().has_value());
    }

    void winsAtTarget()
    {
        BummerlScore score;
        for (int i = 0; i < BummerlScore::kTarget - 1; ++i) {
            score.addHandWin(Team::TeamB);
        }
        QVERIFY(!score.isWon());

        score.addHandWin(Team::TeamB);
        QVERIFY(score.isWon());
        QVERIFY(score.winner().has_value());
        QCOMPARE(*score.winner(), Team::TeamB);
        QCOMPARE(score.score(Team::TeamB), BummerlScore::kTarget);
    }

    void scoreDoesNotAdvanceOnceWon()
    {
        BummerlScore score;
        for (int i = 0; i < BummerlScore::kTarget; ++i) {
            score.addHandWin(Team::TeamA);
        }
        score.addHandWin(Team::TeamA);
        score.addHandWin(Team::TeamB);

        QCOMPARE(score.score(Team::TeamA), BummerlScore::kTarget);
        QCOMPARE(score.score(Team::TeamB), 0);
    }
};

QTEST_MAIN(TestBummerlScore)
#include "test_bummerl_score.moc"
