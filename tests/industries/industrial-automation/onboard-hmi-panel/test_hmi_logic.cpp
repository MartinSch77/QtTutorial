// SPDX-License-Identifier: MIT
#include "AlarmLogModel.h"
#include "ProcessSimulator.h"
#include "PumpController.h"

#include <QCoreApplication>
#include <QSignalSpy>
#include <QTest>

#include <cmath>

using namespace qttutorial::industrial_hmi;

class TestHmiLogic : public QObject {
    Q_OBJECT
private slots:
    void tankLevelIntegratesFlowRatherThanJumping()
    {
        ProcessSimulator sim;
        sim.setPumpRunning(true);
        const double startLevel = sim.tankLevel();
        sim.tick(0.1);
        const double afterOneTick = sim.tankLevel();
        QVERIFY(std::abs(afterOneTick - startLevel) < 5.0);
    }

    void flowRampsUpGraduallyWhenPumpStarts()
    {
        ProcessSimulator sim;
        sim.setPumpRunning(true);
        sim.tick(0.1);
        const double flowAfterOneTick = sim.flowRate();
        QVERIFY(flowAfterOneTick > 0.0);
        QVERIFY(flowAfterOneTick < ProcessSimulator::kMaxInflowRate);
    }

    void tankLevelDrainsWhenPumpOffAndAboveOutflowBaseline()
    {
        ProcessSimulator sim;
        sim.setPumpRunning(false);
        for (int i = 0; i < 50; ++i) {
            sim.tick(0.1);
        }
        QVERIFY(sim.tankLevel() < 45.0);
    }

    void highLevelAlarmFiresAboveThreshold()
    {
        ProcessSimulator sim;
        QSignalSpy spy(&sim, &ProcessSimulator::highLevelAlarm);
        sim.setPumpRunning(true);
        for (int i = 0; i < 2000 && spy.isEmpty(); ++i) {
            sim.tick(0.1);
        }
        QVERIFY(!spy.isEmpty());
        QVERIFY(sim.tankLevel() >= ProcessSimulator::kHighLevelAlarm);
    }

    void closingValveStopsDownstreamOutflowSoLevelStopsFalling()
    {
        ProcessSimulator sim;
        sim.setPumpRunning(false);
        sim.setValveOpen(false);
        for (int i = 0; i < 50; ++i) {
            sim.tick(0.1);
        }
        // With the pump off and the valve closed, nothing is draining out of
        // the tank downstream, so the level should hold rather than fall
        // toward the outflow baseline the way it does with the valve open.
        QCOMPARE(sim.tankLevel(), 45.0);
    }

    void motorOnlyRunsWhenPumpFeedsFlowAndValveIsOpen()
    {
        ProcessSimulator sim;
        sim.setPumpRunning(true);
        sim.setValveOpen(true);
        for (int i = 0; i < 50; ++i) {
            sim.tick(0.1);
        }
        QVERIFY(sim.isMotorRunning());
        QVERIFY(sim.conveyorSpeed() > 0.0);

        sim.setValveOpen(false);
        for (int i = 0; i < 50; ++i) {
            sim.tick(0.1);
        }
        QVERIFY(!sim.isMotorRunning());
        QVERIFY(sim.conveyorSpeed() < 0.05);
    }

    void conveyorSpeedRampsRatherThanJumping()
    {
        ProcessSimulator sim;
        sim.setPumpRunning(true);
        sim.setValveOpen(true);
        sim.tick(0.1);
        QVERIFY(sim.conveyorSpeed() > 0.0);
        QVERIFY(sim.conveyorSpeed() < ProcessSimulator::kMaxConveyorSpeed);
    }

    void pumpStartsAndStopsThroughStateMachine()
    {
        PumpController pump;
        QCoreApplication::processEvents();
        QCOMPARE(pump.state(), PumpController::State::Idle);

        pump.start();
        QCoreApplication::processEvents();
        QCOMPARE(pump.state(), PumpController::State::Running);

        pump.stop();
        QCoreApplication::processEvents();
        QCOMPARE(pump.state(), PumpController::State::Idle);
    }

    void pumpGuardBlocksStartWhenConditionUnsafe()
    {
        PumpController pump;
        pump.setStartGuard([] { return false; });
        QCoreApplication::processEvents();

        pump.start();
        QCoreApplication::processEvents();
        QCOMPARE(pump.state(), PumpController::State::Idle);
    }

    void pumpFaultRequiresExplicitReset()
    {
        PumpController pump;
        QCoreApplication::processEvents();

        pump.start();
        QCoreApplication::processEvents();
        QCOMPARE(pump.state(), PumpController::State::Running);

        pump.raiseFault();
        QCoreApplication::processEvents();
        QCOMPARE(pump.state(), PumpController::State::Fault);

        pump.start();
        QCoreApplication::processEvents();
        QCOMPARE(pump.state(), PumpController::State::Fault);

        pump.reset();
        QCoreApplication::processEvents();
        QCOMPARE(pump.state(), PumpController::State::Idle);
    }

    void alarmLogKeepsNewestFirstAndTracksCount()
    {
        AlarmLogModel log;
        QCOMPARE(log.count(), 0);

        log.raise(QStringLiteral("first"), AlarmLogModel::Severity::Info);
        log.raise(QStringLiteral("second"), AlarmLogModel::Severity::Critical);

        QCOMPARE(log.count(), 2);
        QCOMPARE(log.latestMessage(), QStringLiteral("second"));
        QCOMPARE(log.latestSeverity(), static_cast<int>(AlarmLogModel::Severity::Critical));
        QCOMPARE(log.data(log.index(1, 0), AlarmLogModel::MessageRole).toString(), QStringLiteral("first"));
    }

    void acknowledgingAnAlarmClearsItsFlagAndUpdatesTheOutstandingCount()
    {
        AlarmLogModel log;
        log.raise(QStringLiteral("first"), AlarmLogModel::Severity::Warning);
        log.raise(QStringLiteral("second"), AlarmLogModel::Severity::Critical);
        QCOMPARE(log.unacknowledgedCount(), 2);

        log.acknowledge(0);
        QCOMPARE(log.unacknowledgedCount(), 1);
        QVERIFY(log.data(log.index(0, 0), AlarmLogModel::AcknowledgedRole).toBool());
        QVERIFY(!log.data(log.index(1, 0), AlarmLogModel::AcknowledgedRole).toBool());

        log.acknowledgeAll();
        QCOMPARE(log.unacknowledgedCount(), 0);
    }
};

QTEST_MAIN(TestHmiLogic)
#include "test_hmi_logic.moc"
