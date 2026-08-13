// SPDX-License-Identifier: MIT
#include "DeckLifeSupportMachine.h"

#include <utility>

namespace qttutorial::startrek_bridge {

DeckLifeSupportMachine::DeckLifeSupportMachine(QString deckName, double nominalLow, double criticalLow,
                                                QObject* parent)
    : QObject(parent)
    , m_deckName(std::move(deckName))
    , m_monitor(nominalLow, criticalLow)
{
    m_machine.addState(&m_nominalState);
    m_machine.addState(&m_cautionState);
    m_machine.addState(&m_criticalState);
    m_machine.setInitialState(&m_nominalState);

    m_nominalState.addTransition(&m_monitor, &DeckLifeSupportMonitor::cautionDetected, &m_cautionState);
    m_nominalState.addTransition(&m_monitor, &DeckLifeSupportMonitor::criticalDetected, &m_criticalState);
    m_cautionState.addTransition(&m_monitor, &DeckLifeSupportMonitor::nominalDetected, &m_nominalState);
    m_cautionState.addTransition(&m_monitor, &DeckLifeSupportMonitor::criticalDetected, &m_criticalState);
    m_criticalState.addTransition(&m_monitor, &DeckLifeSupportMonitor::nominalDetected, &m_nominalState);
    m_criticalState.addTransition(&m_monitor, &DeckLifeSupportMonitor::cautionDetected, &m_cautionState);

    connect(&m_nominalState, &QState::entered, this, [this] {
        m_currentStateName = QStringLiteral("Nominal");
        emit statusChanged(m_deckName, m_currentStateName);
    });
    connect(&m_cautionState, &QState::entered, this, [this] {
        m_currentStateName = QStringLiteral("Caution");
        emit statusChanged(m_deckName, m_currentStateName);
    });
    connect(&m_criticalState, &QState::entered, this, [this] {
        m_currentStateName = QStringLiteral("Critical");
        emit statusChanged(m_deckName, m_currentStateName);
    });
}

void DeckLifeSupportMachine::start()
{
    m_machine.start();
}

void DeckLifeSupportMachine::updateValue(double value)
{
    m_monitor.evaluate(value);
}

} // namespace qttutorial::startrek_bridge
