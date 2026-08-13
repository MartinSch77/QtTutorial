// SPDX-License-Identifier: MIT
#include "SubsystemHealthMachine.h"

#include <utility>

namespace qttutorial::space {

SubsystemHealthMachine::SubsystemHealthMachine(QString subsystemName, double nominalLow, double nominalHigh,
                                                double criticalLow, double criticalHigh, QObject* parent)
    : QObject(parent)
    , m_subsystemName(std::move(subsystemName))
    , m_monitor(nominalLow, nominalHigh, criticalLow, criticalHigh)
{
    m_machine.addState(&m_nominalState);
    m_machine.addState(&m_cautionState);
    m_machine.addState(&m_criticalState);
    m_machine.setInitialState(&m_nominalState);

    m_nominalState.addTransition(&m_monitor, &SubsystemMonitor::cautionDetected, &m_cautionState);
    m_nominalState.addTransition(&m_monitor, &SubsystemMonitor::criticalDetected, &m_criticalState);
    m_cautionState.addTransition(&m_monitor, &SubsystemMonitor::nominalDetected, &m_nominalState);
    m_cautionState.addTransition(&m_monitor, &SubsystemMonitor::criticalDetected, &m_criticalState);
    m_criticalState.addTransition(&m_monitor, &SubsystemMonitor::nominalDetected, &m_nominalState);
    m_criticalState.addTransition(&m_monitor, &SubsystemMonitor::cautionDetected, &m_cautionState);

    connect(&m_nominalState, &QState::entered, this, [this] {
        m_currentStateName = QStringLiteral("Nominal");
        emit healthChanged(m_subsystemName, m_currentStateName);
    });
    connect(&m_cautionState, &QState::entered, this, [this] {
        m_currentStateName = QStringLiteral("Caution");
        emit healthChanged(m_subsystemName, m_currentStateName);
    });
    connect(&m_criticalState, &QState::entered, this, [this] {
        m_currentStateName = QStringLiteral("Critical");
        emit healthChanged(m_subsystemName, m_currentStateName);
    });
}

void SubsystemHealthMachine::start()
{
    m_machine.start();
}

void SubsystemHealthMachine::updateValue(double value)
{
    m_monitor.evaluate(value);
}

} // namespace qttutorial::space
