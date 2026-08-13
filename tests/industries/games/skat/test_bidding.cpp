// SPDX-License-Identifier: MIT
#include "Bidding.h"

#include <QTest>

using namespace qttutorial::games::skat;

class TestBidding : public QObject {
    Q_OBJECT
private slots:
    void endsWithExactlyOneBidderRemainingAfterTwoPasses()
    {
        Bidding bidding({0, 1, 2}, 18);

        QVERIFY(!bidding.isFinished());
        QCOMPARE(bidding.currentTurnSeat(), 0);

        QVERIFY(bidding.submitBid(0, 18).has_value());
        QVERIFY(bidding.submitPass(1).has_value());
        QVERIFY(!bidding.isFinished());
        QCOMPARE(bidding.currentTurnSeat(), 2);

        QVERIFY(bidding.submitPass(2).has_value());
        QVERIFY(bidding.isFinished());
        QCOMPARE(bidding.declarerSeat().value(), 0);
        QCOMPARE(bidding.declarerValue(), 18);
    }

    void rejectsOutOfTurnAction()
    {
        Bidding bidding({0, 1, 2}, 18);
        const auto result = bidding.submitBid(1, 18);
        QVERIFY(!result.has_value());
        QCOMPARE(result.error(), Bidding::ActionError::NotYourTurn);
    }

    void rejectsBidNotStrictlyHigherThanCurrent()
    {
        Bidding bidding({0, 1, 2}, 18);
        QVERIFY(bidding.submitBid(0, 18).has_value());
        const auto tooLow = bidding.submitBid(1, 18);
        QVERIFY(!tooLow.has_value());
        QCOMPARE(tooLow.error(), Bidding::ActionError::BidTooLow);

        const auto passInstead = bidding.submitPass(1);
        QVERIFY(passInstead.has_value());
    }

    void rotatesThroughRemainingBiddersAfterAPass()
    {
        Bidding bidding({0, 1, 2}, 18);
        QVERIFY(bidding.submitPass(0).has_value());
        QCOMPARE(bidding.currentTurnSeat(), 1);
        QVERIFY(bidding.submitBid(1, 18).has_value());
        QCOMPARE(bidding.currentTurnSeat(), 2);
        QVERIFY(bidding.submitBid(2, 19).has_value());
        QCOMPARE(bidding.currentTurnSeat(), 1);
    }

    void lastRemainingPlayerBecomesDeclarerAtBaselineIfNobodyEverBid()
    {
        Bidding bidding({0, 1, 2}, 18);
        QVERIFY(bidding.submitPass(0).has_value());
        QVERIFY(bidding.submitPass(1).has_value());
        QVERIFY(bidding.isFinished());
        QCOMPARE(bidding.declarerSeat().value(), 2);
        QCOMPARE(bidding.declarerValue(), 18);
        QCOMPARE(bidding.highestBidder(), -1);
    }

    void cannotActAfterFinished()
    {
        Bidding bidding({0, 1, 2}, 18);
        QVERIFY(bidding.submitPass(0).has_value());
        QVERIFY(bidding.submitPass(1).has_value());
        QVERIFY(bidding.isFinished());
        const auto result = bidding.submitPass(2);
        QVERIFY(!result.has_value());
        QCOMPARE(result.error(), Bidding::ActionError::AlreadyFinished);
    }
};

QTEST_APPLESS_MAIN(TestBidding)
#include "test_bidding.moc"
