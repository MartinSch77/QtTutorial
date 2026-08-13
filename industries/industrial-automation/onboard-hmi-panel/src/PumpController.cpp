// SPDX-License-Identifier: MIT
#include "PumpController.h"

namespace qttutorial::industrial_hmi {

GuardedSignalTransition::GuardedSignalTransition(QObject* sender, const char* signal, std::function<bool()> guard)
    : QSignalTransition(sender, signal)
    , m_guard(std::move(guard))
{
}

bool GuardedSignalTransition::eventTest(QEvent* event)
{
    return QSignalTransition::eventTest(event) && m_guard();
}

PumpController::PumpController(QObject* parent)
    : QObject(parent)
    , m_idle(new QState(&m_machine))
    , m_running(new QState(&m_machine))
    , m_fault(new QState(&m_machine))
{
    auto* startTransition = new GuardedSignalTransition(this, SIGNAL(started()), [this] { return m_startGuard(); });
    startTransition->setTargetState(m_running);
    m_idle->addTransition(startTransition);

    m_running->addTransition(this, &PumpController::stopped, m_idle);

    m_idle->addTransition(this, &PumpController::faultRaised, m_fault);
    m_running->addTransition(this, &PumpController::faultRaised, m_fault);

    m_fault->addTransition(this, &PumpController::resetRequested, m_idle);

    connect(m_idle, &QState::entered, this, [this] { enterState(State::Idle); });
    connect(m_running, &QState::entered, this, [this] { enterState(State::Running); });
    connect(m_fault, &QState::entered, this, [this] { enterState(State::Fault); });

    m_machine.setInitialState(m_idle);
    m_machine.start();
}

void PumpController::setStartGuard(std::function<bool()> guard)
{
    m_startGuard = std::move(guard);
}

void PumpController::start()
{
    emit started();
}

void PumpController::stop()
{
    emit stopped();
}

void PumpController::raiseFault()
{
    emit faultRaised();
}

void PumpController::reset()
{
    emit resetRequested();
}

void PumpController::enterState(State state)
{
    m_state = state;
    emit stateChanged(m_state);
}

} // namespace qttutorial::industrial_hmi
