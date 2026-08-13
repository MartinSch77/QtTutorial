// SPDX-License-Identifier: MIT
#include "MachineStateController.h"

namespace qttutorial::factory_machine_panel {

MachineStateController::MachineStateController(QObject* parent)
    : QObject(parent)
    , m_idle(new QState(&m_machine))
    , m_running(new QState(&m_machine))
    , m_paused(new QState(&m_machine))
    , m_warning(new QState(&m_machine))
    , m_fault(new QState(&m_machine))
{
    m_idle->addTransition(this, &MachineStateController::started, m_running);

    m_running->addTransition(this, &MachineStateController::stopped, m_idle);
    m_running->addTransition(this, &MachineStateController::paused, m_paused);
    m_running->addTransition(this, &MachineStateController::warningRaised, m_warning);
    m_running->addTransition(this, &MachineStateController::faultRaised, m_fault);

    m_paused->addTransition(this, &MachineStateController::started, m_running);
    m_paused->addTransition(this, &MachineStateController::stopped, m_idle);

    m_warning->addTransition(this, &MachineStateController::warningCleared, m_running);
    m_warning->addTransition(this, &MachineStateController::stopped, m_idle);
    m_warning->addTransition(this, &MachineStateController::faultRaised, m_fault);

    m_fault->addTransition(this, &MachineStateController::acknowledged, m_idle);

    connect(m_idle, &QState::entered, this, [this] { enterState(State::Idle); });
    connect(m_running, &QState::entered, this, [this] { enterState(State::Running); });
    connect(m_paused, &QState::entered, this, [this] { enterState(State::Paused); });
    connect(m_warning, &QState::entered, this, [this] { enterState(State::Warning); });
    connect(m_fault, &QState::entered, this, [this] { enterState(State::Fault); });

    m_machine.setInitialState(m_idle);
    m_machine.start();
}

void MachineStateController::start()
{
    emit started();
}

void MachineStateController::stop()
{
    emit stopped();
}

void MachineStateController::pause()
{
    emit paused();
}

void MachineStateController::raiseWarning()
{
    emit warningRaised();
}

void MachineStateController::clearWarning()
{
    emit warningCleared();
}

void MachineStateController::raiseFault()
{
    emit faultRaised();
}

void MachineStateController::acknowledge()
{
    emit acknowledged();
}

void MachineStateController::enterState(State state)
{
    m_state = state;
    emit stateChanged(m_state);
}

} // namespace qttutorial::factory_machine_panel
