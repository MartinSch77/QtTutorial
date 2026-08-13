// SPDX-License-Identifier: GPL-3.0-or-later
#include "MachineTelemetrySimulator.h"

#include <QSignalSpy>
#include <QTest>

using qttutorial::offboard_digital_twin::MachineState;
using qttutorial::offboard_digital_twin::MachineTelemetrySimulator;

class TestMachineTelemetrySimulator : public QObject {
    Q_OBJECT
private slots:
    void startsWithFourMachinesInNormalState()
    {
        MachineTelemetrySimulator sim;
        QCOMPARE(sim.machineCount(), 4);
        for (int i = 0; i < sim.machineCount(); ++i)
            QCOMPARE(static_cast<int>(sim.machineAt(i).state), static_cast<int>(MachineState::Normal));
    }

    void indexOfIdFindsAndRejects()
    {
        MachineTelemetrySimulator sim;
        QCOMPARE(sim.indexOfId(1), 1);
        QCOMPARE(sim.indexOfId(999), -1);
    }

    void tickEmitsTelemetryChangedForEveryRunningMachine()
    {
        MachineTelemetrySimulator sim;
        QSignalSpy spy(&sim, &MachineTelemetrySimulator::machineTelemetryChanged);
        sim.tick(0.2);
        QCOMPARE(spy.count(), sim.machineCount());
    }

    void overheatRampsTemperatureIntoAlarmAndEmitsStateChange()
    {
        MachineTelemetrySimulator sim;
        const int machineId = 1;
        sim.beginOverheat(machineId);

        QSignalSpy stateSpy(&sim, &MachineTelemetrySimulator::machineStateChanged);
        for (int i = 0; i < 60; ++i) // 60 * 0.2s = 12s, comfortably past the 6s ramp
            sim.tick(0.2);

        const int idx = sim.indexOfId(machineId);
        QVERIFY(idx >= 0);
        QCOMPARE(static_cast<int>(sim.machineAt(idx).state), static_cast<int>(MachineState::Alarm));
        QVERIFY(sim.machineAt(idx).temperatureC > 90.0);
        QVERIFY(stateSpy.count() > 0);
    }

    void clearFaultResetsMachineToNormal()
    {
        MachineTelemetrySimulator sim;
        const int machineId = 1;
        sim.beginOverheat(machineId);
        for (int i = 0; i < 60; ++i)
            sim.tick(0.2);

        sim.clearFault(machineId);
        const int idx = sim.indexOfId(machineId);
        QCOMPARE(static_cast<int>(sim.machineAt(idx).state), static_cast<int>(MachineState::Normal));
        QVERIFY(sim.machineAt(idx).temperatureC < 60.0);
    }

    void stoppedMachineDoesNotDrift()
    {
        MachineTelemetrySimulator sim;
        const int machineId = 0;
        sim.setStopped(machineId, true);
        const int idx = sim.indexOfId(machineId);
        const double tempBefore = sim.machineAt(idx).temperatureC;

        for (int i = 0; i < 20; ++i)
            sim.tick(0.2);

        QCOMPARE(sim.machineAt(idx).temperatureC, tempBefore);
        QCOMPARE(static_cast<int>(sim.machineAt(idx).state), static_cast<int>(MachineState::Stopped));
    }

    void setStoppedFalseEntersMaintenanceWithDefectivePart()
    {
        MachineTelemetrySimulator sim;
        const int machineId = 0;
        sim.setStopped(machineId, false);
        const int idx = sim.indexOfId(machineId);
        QCOMPARE(static_cast<int>(sim.machineAt(idx).state), static_cast<int>(MachineState::Maintenance));
        QVERIFY(!sim.machineAt(idx).defectivePart.isEmpty());
    }
};

QTEST_MAIN(TestMachineTelemetrySimulator)
#include "test_machine_telemetry_simulator.moc"
