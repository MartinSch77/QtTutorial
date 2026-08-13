// SPDX-License-Identifier: MIT
#pragma once

#include "EventLogModel.h"
#include "MachineSimulator.h"
#include "MachineStateController.h"

#include <QObject>

namespace qttutorial::factory_machine_panel {

// Wires the testable logic classes together for the running application:
// the simulator only spins while the machine is Running or Warning, sensor
// excursions raise/clear the Warning state and escalate to Fault, and every
// state transition and sensor excursion is recorded to the event log.
// Exposed to QML as context properties by main.cpp. This glue class mirrors
// the wiring pattern of the other onboard-* apps in this tutorial (compare
// HmiApp in industrial-automation/onboard-hmi-panel) and, like those, is
// exercised through the app itself rather than via QTest, since the
// individually testable behaviour lives in MachineSimulator and
// MachineStateController.
class MachinePanelApp : public QObject {
    Q_OBJECT
public:
    explicit MachinePanelApp(QObject* parent = nullptr);

    [[nodiscard]] MachineSimulator* simulator() { return &m_simulator; }
    [[nodiscard]] MachineStateController* controller() { return &m_controller; }
    [[nodiscard]] EventLogModel* eventLog() { return &m_eventLog; }

public slots:
    void requestStart();
    void requestStop();
    void requestPause();
    void requestAcknowledge();
    bool requestSetpoint(double rpm);

private:
    enum class HealthLevel { Ok, Warning, Fault };

    void evaluateHealth();

    MachineSimulator m_simulator;
    MachineStateController m_controller;
    EventLogModel m_eventLog;
    HealthLevel m_healthLevel = HealthLevel::Ok;
};

} // namespace qttutorial::factory_machine_panel
