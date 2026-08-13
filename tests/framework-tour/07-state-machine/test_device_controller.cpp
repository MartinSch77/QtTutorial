// SPDX-License-Identifier: MIT
#include "DeviceController.h"

#include <QSignalSpy>
#include <QTest>

using qttutorial::state_machine::DeviceController;

namespace {

// QStateMachine::start() enters the initial state asynchronously, through the event
// loop, not synchronously when the controller is constructed. Every test must wait
// for that initial entry before sending any signal, otherwise the signal can fire
// before the machine has any active state listening for it and gets silently
// dropped.
void waitForIdle(DeviceController& controller)
{
    QTRY_COMPARE(controller.currentState(), QStringLiteral("Idle"));
}

// Drives a freshly constructed, already-idle controller from Idle all the way to
// Closed, clearing the interlock so the guarded Closed -> Opening transition does
// not get in the way while setting up a scenario. Every step is confirmed with
// QTRY_COMPARE because QStateMachine transitions are processed asynchronously
// through the event loop, never synchronously with the signal that triggers them.
void driveToClosed(DeviceController& controller)
{
    waitForIdle(controller);
    controller.setInterlockCleared(true);

    controller.requestOpen();
    QTRY_COMPARE(controller.currentState(), QStringLiteral("Opening"));
    QTRY_COMPARE(controller.currentState(), QStringLiteral("Open"));

    controller.requestClose();
    QTRY_COMPARE(controller.currentState(), QStringLiteral("Closing"));
    QTRY_COMPARE(controller.currentState(), QStringLiteral("Closed"));
}

} // namespace

class TestDeviceController : public QObject {
    Q_OBJECT
private slots:
    void startsInIdle()
    {
        DeviceController controller;
        QTRY_COMPARE(controller.currentState(), QStringLiteral("Idle"));
    }

    void idleOpensThroughOpeningIntoOpen()
    {
        DeviceController controller;
        waitForIdle(controller);
        QSignalSpy stateChangedSpy(&controller, &DeviceController::stateChanged);

        controller.requestOpen();
        QTRY_COMPARE(controller.currentState(), QStringLiteral("Opening"));
        QTRY_COMPARE(controller.currentState(), QStringLiteral("Open"));

        QVERIFY(stateChangedSpy.count() >= 2);
    }

    void openClosesThroughClosingIntoClosed()
    {
        DeviceController controller;
        driveToClosed(controller);
        QCOMPARE(controller.currentState(), QStringLiteral("Closed"));
    }

    void errorFromOpenLandsInFault()
    {
        DeviceController controller;
        waitForIdle(controller);
        controller.requestOpen();
        QTRY_COMPARE(controller.currentState(), QStringLiteral("Open"));

        controller.error();
        QTRY_COMPARE(controller.currentState(), QStringLiteral("Fault"));
    }

    void errorFromOpeningLandsInFault()
    {
        DeviceController controller;
        waitForIdle(controller);
        controller.requestOpen();
        QTRY_COMPARE(controller.currentState(), QStringLiteral("Opening"));

        controller.error();
        QTRY_COMPARE(controller.currentState(), QStringLiteral("Fault"));
    }

    void resetFromFaultReturnsToIdle()
    {
        DeviceController controller;
        waitForIdle(controller);
        controller.error();
        QTRY_COMPARE(controller.currentState(), QStringLiteral("Fault"));

        controller.reset();
        QTRY_COMPARE(controller.currentState(), QStringLiteral("Idle"));
    }

    void guardedOpenIsRejectedWhenInterlockNotCleared()
    {
        DeviceController controller;
        driveToClosed(controller);
        QCOMPARE(controller.currentState(), QStringLiteral("Closed"));

        controller.setInterlockCleared(false);
        QSignalSpy rejectedSpy(&controller, &DeviceController::openRequestRejected);

        controller.requestOpen();
        QCOMPARE(rejectedSpy.count(), 1);

        // Give the state machine's event loop plenty of time to (not) transition.
        QTest::qWait(500);
        QCOMPARE(controller.currentState(), QStringLiteral("Closed"));
    }

    void guardedOpenSucceedsWhenInterlockCleared()
    {
        DeviceController controller;
        driveToClosed(controller);
        QCOMPARE(controller.currentState(), QStringLiteral("Closed"));

        controller.setInterlockCleared(true);
        QSignalSpy rejectedSpy(&controller, &DeviceController::openRequestRejected);

        controller.requestOpen();
        QTRY_COMPARE(controller.currentState(), QStringLiteral("Opening"));
        QTRY_COMPARE(controller.currentState(), QStringLiteral("Open"));

        QCOMPARE(rejectedSpy.count(), 0);
    }
};

QTEST_MAIN(TestDeviceController)
#include "test_device_controller.moc"
