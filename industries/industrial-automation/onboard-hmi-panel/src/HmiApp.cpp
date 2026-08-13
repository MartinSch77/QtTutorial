// SPDX-License-Identifier: MIT
#include "HmiApp.h"

namespace qttutorial::industrial_hmi {

HmiApp::HmiApp(QObject* parent)
    : QObject(parent)
{
    m_pump.setStartGuard([this] { return m_process.tankLevel() < ProcessSimulator::kHighLevelAlarm; });

    connect(&m_pump, &PumpController::stateChanged, this, [this](PumpController::State state) {
        m_process.setPumpRunning(state == PumpController::State::Running);
    });

    connect(&m_process, &ProcessSimulator::highLevelAlarm, this, [this] {
        m_alarms.raise(tr("Tank level critically high"), AlarmLogModel::Severity::Critical);
        m_pump.raiseFault();
    });
    connect(&m_process, &ProcessSimulator::lowLevelAlarm, this,
            [this] { m_alarms.raise(tr("Tank level low"), AlarmLogModel::Severity::Warning); });
    connect(&m_process, &ProcessSimulator::highPressureAlarm, this,
            [this] { m_alarms.raise(tr("Pipe pressure above limit"), AlarmLogModel::Severity::Warning); });
    connect(&m_pump, &PumpController::faultRaised, this,
            [this] { m_alarms.raise(tr("Pump tripped to Fault"), AlarmLogModel::Severity::Critical); });
}

void HmiApp::togglePump()
{
    if (m_pump.state() == PumpController::State::Running) {
        m_pump.stop();
    } else if (m_pump.state() == PumpController::State::Idle) {
        m_pump.start();
    } else {
        m_pump.reset();
    }
}

} // namespace qttutorial::industrial_hmi
