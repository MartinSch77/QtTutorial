// SPDX-License-Identifier: MIT
#include "OrderLifecycle.h"

#include <QSignalSpy>
#include <QTest>

using qttutorial::stock_tracker::OrderLifecycle;

namespace {

// QStateMachine::start() enters its initial state asynchronously, through the
// event loop, not synchronously when OrderLifecycle is constructed. Every test
// must wait for that initial entry before calling submit()/fill()/etc,
// otherwise the triggering signal can be emitted before the machine has any
// active state listening for it and gets silently dropped.
void waitForDraft(OrderLifecycle& order)
{
    QTRY_COMPARE(order.currentState(), QStringLiteral("Draft"));
}

void driveToSubmitted(OrderLifecycle& order)
{
    waitForDraft(order);
    order.setSymbol(QStringLiteral("QTX"));
    order.setQuantity(100);
    order.submit();
    QTRY_COMPARE(order.currentState(), QStringLiteral("Submitted"));
}

} // namespace

class TestOrderLifecycle : public QObject {
    Q_OBJECT
private slots:
    void startsInDraft()
    {
        OrderLifecycle order;
        QTRY_COMPARE(order.currentState(), QStringLiteral("Draft"));
    }

    void submitWithEmptySymbolIsRejected()
    {
        OrderLifecycle order;
        waitForDraft(order);
        order.setQuantity(100);
        QVERIFY(!order.isValid());

        order.submit();
        // Give the state machine's event loop plenty of time to (not) transition.
        QTest::qWait(300);
        QCOMPARE(order.currentState(), QStringLiteral("Draft"));
    }

    void submitWithZeroQuantityIsRejected()
    {
        OrderLifecycle order;
        waitForDraft(order);
        order.setSymbol(QStringLiteral("QTX"));
        order.setQuantity(0);
        QVERIFY(!order.isValid());

        order.submit();
        QTest::qWait(300);
        QCOMPARE(order.currentState(), QStringLiteral("Draft"));
    }

    void submitWithValidOrderSucceeds()
    {
        OrderLifecycle order;
        driveToSubmitted(order);
        QCOMPARE(order.currentState(), QStringLiteral("Submitted"));
    }

    void submittedOrderCanBeFilled()
    {
        OrderLifecycle order;
        driveToSubmitted(order);

        order.fill();
        QTRY_COMPARE(order.currentState(), QStringLiteral("Filled"));
    }

    void submittedOrderCanBeCancelled()
    {
        OrderLifecycle order;
        driveToSubmitted(order);

        order.cancel();
        QTRY_COMPARE(order.currentState(), QStringLiteral("Cancelled"));
    }

    void submittedOrderCanBeRejected()
    {
        OrderLifecycle order;
        driveToSubmitted(order);

        order.reject();
        QTRY_COMPARE(order.currentState(), QStringLiteral("Rejected"));
    }

    void cancelAfterAlreadyFilledIsRejected()
    {
        OrderLifecycle order;
        driveToSubmitted(order);
        order.fill();
        QTRY_COMPARE(order.currentState(), QStringLiteral("Filled"));

        QSignalSpy stateChangedSpy(&order, &OrderLifecycle::stateChanged);
        order.cancel();
        QTest::qWait(300);

        QCOMPARE(order.currentState(), QStringLiteral("Filled"));
        QCOMPARE(stateChangedSpy.count(), 0);
    }

    void resetFromFilledReturnsToDraft()
    {
        OrderLifecycle order;
        driveToSubmitted(order);
        order.fill();
        QTRY_COMPARE(order.currentState(), QStringLiteral("Filled"));

        order.reset();
        QTRY_COMPARE(order.currentState(), QStringLiteral("Draft"));
        QVERIFY(!order.isValid());
    }
};

QTEST_MAIN(TestOrderLifecycle)
#include "test_order_lifecycle.moc"
