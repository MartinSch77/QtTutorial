// SPDX-License-Identifier: GPL-3.0-or-later
#include "MaintenanceWorkflow.h"

#include <QSignalSpy>
#include <QTest>

using qttutorial::offboard_digital_twin::MaintenanceWorkflow;

class TestMaintenanceWorkflow : public QObject {
    Q_OBJECT
private slots:
    void startsIdleAndNotRunning()
    {
        MaintenanceWorkflow workflow(1);
        QCOMPARE(workflow.stepIndex(), static_cast<int>(MaintenanceWorkflow::Idle));
        QVERIFY(!workflow.isRunning());
    }

    void startBeginsDiagnosing()
    {
        MaintenanceWorkflow workflow(1);
        workflow.start();
        QCOMPARE(workflow.stepIndex(), static_cast<int>(MaintenanceWorkflow::Diagnosing));
        QVERIFY(workflow.isRunning());
    }

    void fullHappyPathReachesComplete()
    {
        MaintenanceWorkflow workflow(1);
        workflow.start();
        QTRY_COMPARE_WITH_TIMEOUT(workflow.stepIndex(), static_cast<int>(MaintenanceWorkflow::Complete), 15000);
        QVERIFY(!workflow.isRunning());
        QVERIFY(!workflow.isInError());
    }

    void cancelWhileRunningReturnsToIdle()
    {
        MaintenanceWorkflow workflow(1);
        QSignalSpy cancelledSpy(&workflow, &MaintenanceWorkflow::cancelled);
        workflow.start();
        QTest::qWait(100);
        workflow.cancel();
        QCOMPARE(workflow.stepIndex(), static_cast<int>(MaintenanceWorkflow::Idle));
        QVERIFY(!workflow.isRunning());
        QCOMPARE(cancelledSpy.count(), 1);
    }

    void injectedFailureEntersErrorState()
    {
        MaintenanceWorkflow workflow(1);
        workflow.injectFailureOnNextStep();
        workflow.start();
        QTRY_COMPARE_WITH_TIMEOUT(workflow.stepIndex(), static_cast<int>(MaintenanceWorkflow::Error), 5000);
        QVERIFY(workflow.isInError());
        QVERIFY(!workflow.errorMessage().isEmpty());
    }

    void retryAfterFailureResumesAndCanFinish()
    {
        MaintenanceWorkflow workflow(1);
        workflow.injectFailureOnNextStep();
        workflow.start();
        QTRY_COMPARE_WITH_TIMEOUT(workflow.stepIndex(), static_cast<int>(MaintenanceWorkflow::Error), 5000);

        workflow.retry();
        QVERIFY(!workflow.isInError());
        QTRY_COMPARE_WITH_TIMEOUT(workflow.stepIndex(), static_cast<int>(MaintenanceWorkflow::Complete), 15000);
    }

    void cancelFromErrorStateReturnsToIdle()
    {
        MaintenanceWorkflow workflow(1);
        workflow.injectFailureOnNextStep();
        workflow.start();
        QTRY_COMPARE_WITH_TIMEOUT(workflow.stepIndex(), static_cast<int>(MaintenanceWorkflow::Error), 5000);

        workflow.cancel();
        QCOMPARE(workflow.stepIndex(), static_cast<int>(MaintenanceWorkflow::Idle));
        QVERIFY(!workflow.isInError());
    }
};

QTEST_MAIN(TestMaintenanceWorkflow)
#include "test_maintenance_workflow.moc"
