// SPDX-License-Identifier: MIT
#include "MachinePanelApp.h"

namespace qttutorial::factory_machine_panel {

MachinePanelApp::MachinePanelApp(QObject* parent)
    : QObject(parent)
{
    connect(&m_controller, &MachineStateController::stateChanged, this,
            [this](MachineStateController::State state) {
                const bool shouldSpin =
                    state == MachineStateController::State::Running || state == MachineStateController::State::Warning;
                m_simulator.setRunning(shouldSpin);

                switch (state) {
                case MachineStateController::State::Idle:
                    m_eventLog.record(tr("Machine idle"), EventLogModel::Severity::Info);
                    break;
                case MachineStateController::State::Running:
                    m_eventLog.record(tr("Machine running"), EventLogModel::Severity::Info);
                    break;
                case MachineStateController::State::Paused:
                    m_eventLog.record(tr("Machine paused"), EventLogModel::Severity::Info);
                    break;
                case MachineStateController::State::Warning:
                    m_eventLog.record(tr("Machine condition degraded"), EventLogModel::Severity::Warning);
                    break;
                case MachineStateController::State::Fault:
                    m_eventLog.record(tr("Machine fault - production stopped"), EventLogModel::Severity::Fault);
                    break;
                }
            });

    connect(&m_simulator, &MachineSimulator::temperatureChanged, this, [this] { evaluateHealth(); });
    connect(&m_simulator, &MachineSimulator::vibrationChanged, this, [this] { evaluateHealth(); });
    connect(&m_simulator, &MachineSimulator::setpointRejected, this,
            [this](const QString& reason) { m_eventLog.record(reason, EventLogModel::Severity::Warning); });
}

void MachinePanelApp::requestStart()
{
    m_controller.start();
}

void MachinePanelApp::requestStop()
{
    m_controller.stop();
}

void MachinePanelApp::requestPause()
{
    m_controller.pause();
}

void MachinePanelApp::requestAcknowledge()
{
    m_controller.acknowledge();
}

bool MachinePanelApp::requestSetpoint(double rpm)
{
    return m_simulator.setTargetSpeed(rpm);
}

void MachinePanelApp::evaluateHealth()
{
    const bool isFault = m_simulator.temperature() >= MachineSimulator::kTempFaultThreshold
        || m_simulator.vibration() >= MachineSimulator::kVibrationFaultThreshold;
    const bool isWarning = !isFault
        && (m_simulator.temperature() >= MachineSimulator::kTempWarningThreshold
            || m_simulator.vibration() >= MachineSimulator::kVibrationWarningThreshold);

    if (isFault) {
        if (m_healthLevel != HealthLevel::Fault) {
            m_healthLevel = HealthLevel::Fault;
            m_controller.raiseFault();
        }
        return;
    }

    if (isWarning) {
        if (m_healthLevel == HealthLevel::Ok) {
            m_healthLevel = HealthLevel::Warning;
            m_controller.raiseWarning();
        }
        return;
    }

    if (m_healthLevel != HealthLevel::Ok) {
        m_healthLevel = HealthLevel::Ok;
        m_controller.clearWarning();
    }
}

} // namespace qttutorial::factory_machine_panel
