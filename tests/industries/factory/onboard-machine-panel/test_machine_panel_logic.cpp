// SPDX-License-Identifier: MIT
#include "EventLogModel.h"
#include "MachineSimulator.h"
#include "MachineStateController.h"

#include <QCoreApplication>
#include <QSignalSpy>
#include <QTest>

using namespace qttutorial::factory_machine_panel;

class TestMachinePanelLogic : public QObject {
    Q_OBJECT
private slots:
    void speedRampsTowardSetpointRatherThanJumping()
    {
        MachineSimulator sim;
        QVERIFY(sim.setTargetSpeed(1000.0));
        sim.setRunning(true);
        sim.tick(0.1);
        QVERIFY(sim.speed() > 0.0);
        QVERIFY(sim.speed() < 1000.0);
    }

    void temperatureDriftsWithinBoundsAtSteadyState()
    {
        MachineSimulator sim;
        QVERIFY(sim.setTargetSpeed(MachineSimulator::kMaxSetpoint));
        sim.setRunning(true);
        for (int i = 0; i < 2000; ++i) {
            sim.tick(0.1);
            // Even with noise, temperature must stay within a sane physical
            // envelope: never below ambient, never wildly beyond the
            // steady-state target implied by the maximum setpoint.
            QVERIFY(sim.temperature() >= MachineSimulator::kAmbientTemperature - 2.0);
            QVERIFY(sim.temperature() <= MachineSimulator::kAmbientTemperature
                    + MachineSimulator::kMaxSetpoint * MachineSimulator::kTempPerRpmUnit + 5.0);
        }
    }

    void vibrationNeverGoesNegative()
    {
        MachineSimulator sim;
        QVERIFY(sim.setTargetSpeed(50.0));
        sim.setRunning(true);
        for (int i = 0; i < 500; ++i) {
            sim.tick(0.1);
            QVERIFY(sim.vibration() >= 0.0);
        }
    }

    void temperatureFaultFiresWhenThresholdExceeded()
    {
        MachineSimulator sim;
        QSignalSpy faultSpy(&sim, &MachineSimulator::temperatureFault);
        QVERIFY(sim.setTargetSpeed(MachineSimulator::kMaxSetpoint));
        sim.setRunning(true);
        for (int i = 0; i < 3000 && faultSpy.isEmpty(); ++i) {
            sim.tick(0.1);
        }
        QVERIFY(!faultSpy.isEmpty());
        QVERIFY(sim.temperature() >= MachineSimulator::kTempFaultThreshold);
    }

    void cycleCountIncrementsAfterEachCycleDurationWhileRunning()
    {
        MachineSimulator sim;
        QVERIFY(sim.setTargetSpeed(500.0));
        sim.setRunning(true);
        QCOMPARE(sim.cycleCount(), 0);

        QSignalSpy cycleSpy(&sim, &MachineSimulator::cycleCompleted);
        // A little over one cycle duration's worth of 0.1s ticks: enough to
        // cross the boundary once, not enough to cross it twice.
        const int ticksPerCycle = static_cast<int>(MachineSimulator::kCycleDurationSeconds / 0.1) + 1;
        for (int i = 0; i < ticksPerCycle; ++i) {
            sim.tick(0.1);
        }
        QCOMPARE(sim.cycleCount(), 1);
        QCOMPARE(cycleSpy.count(), 1);

        for (int i = 0; i < ticksPerCycle; ++i) {
            sim.tick(0.1);
        }
        QCOMPARE(sim.cycleCount(), 2);
    }

    void cycleCountDoesNotAdvanceWhileNotRunning()
    {
        MachineSimulator sim;
        QVERIFY(sim.setTargetSpeed(500.0));
        sim.setRunning(false);
        for (int i = 0; i < 200; ++i) {
            sim.tick(0.1);
        }
        QCOMPARE(sim.cycleCount(), 0);
    }

    void setpointValidationRejectsOutOfRangeValues()
    {
        MachineSimulator sim;
        QSignalSpy rejectedSpy(&sim, &MachineSimulator::setpointRejected);

        QVERIFY(!sim.setTargetSpeed(-10.0));
        QCOMPARE(rejectedSpy.count(), 1);
        QCOMPARE(sim.targetSpeed(), 0.0);

        QVERIFY(!sim.setTargetSpeed(MachineSimulator::kMaxSetpoint + 500.0));
        QCOMPARE(rejectedSpy.count(), 2);

        QVERIFY(sim.setTargetSpeed(1200.0));
        QCOMPARE(sim.targetSpeed(), 1200.0);
    }

    void machineStartsPausesResumesAndStops()
    {
        MachineStateController controller;
        QCoreApplication::processEvents();
        QCOMPARE(controller.state(), MachineStateController::State::Idle);

        controller.start();
        QCoreApplication::processEvents();
        QCOMPARE(controller.state(), MachineStateController::State::Running);

        controller.pause();
        QCoreApplication::processEvents();
        QCOMPARE(controller.state(), MachineStateController::State::Paused);

        controller.start();
        QCoreApplication::processEvents();
        QCOMPARE(controller.state(), MachineStateController::State::Running);

        controller.stop();
        QCoreApplication::processEvents();
        QCOMPARE(controller.state(), MachineStateController::State::Idle);
    }

    void machineEscalatesThroughWarningToFaultAndRequiresAcknowledge()
    {
        MachineStateController controller;
        QCoreApplication::processEvents();

        controller.start();
        QCoreApplication::processEvents();
        QCOMPARE(controller.state(), MachineStateController::State::Running);

        controller.raiseWarning();
        QCoreApplication::processEvents();
        QCOMPARE(controller.state(), MachineStateController::State::Warning);

        controller.clearWarning();
        QCoreApplication::processEvents();
        QCOMPARE(controller.state(), MachineStateController::State::Running);

        controller.raiseWarning();
        QCoreApplication::processEvents();
        controller.raiseFault();
        QCoreApplication::processEvents();
        QCOMPARE(controller.state(), MachineStateController::State::Fault);

        // Fault only clears via an explicit acknowledge, not by starting.
        controller.start();
        QCoreApplication::processEvents();
        QCOMPARE(controller.state(), MachineStateController::State::Fault);

        controller.acknowledge();
        QCoreApplication::processEvents();
        QCOMPARE(controller.state(), MachineStateController::State::Idle);
    }

    void eventLogKeepsNewestFirstAndTracksCount()
    {
        EventLogModel log;
        QCOMPARE(log.count(), 0);

        log.record(QStringLiteral("first"), EventLogModel::Severity::Info);
        log.record(QStringLiteral("second"), EventLogModel::Severity::Fault);

        QCOMPARE(log.count(), 2);
        QCOMPARE(log.latestMessage(), QStringLiteral("second"));
        QCOMPARE(log.latestSeverity(), static_cast<int>(EventLogModel::Severity::Fault));
        QCOMPARE(log.data(log.index(1, 0), EventLogModel::MessageRole).toString(), QStringLiteral("first"));
    }
};

QTEST_MAIN(TestMachinePanelLogic)
#include "test_machine_panel_logic.moc"
