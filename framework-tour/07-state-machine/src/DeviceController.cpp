// SPDX-License-Identifier: MIT
#include "DeviceController.h"

#include "GuardedSignalTransition.h"

#include <QState>
#include <QStateMachine>
#include <QTimer>

namespace qttutorial::state_machine {

namespace {
constexpr int kSimulatedActuationMs = 300;
}

DeviceController::DeviceController(QObject* parent)
    : QObject(parent)
    , m_machine(nullptr)
    , m_operational(nullptr)
    , m_idleState(nullptr)
    , m_openingState(nullptr)
    , m_openState(nullptr)
    , m_closingState(nullptr)
    , m_closedState(nullptr)
    , m_faultState(nullptr)
{
    buildStateMachine();
}

QString DeviceController::currentState() const
{
    return m_currentStateName;
}

bool DeviceController::interlockCleared() const
{
    return m_interlockCleared;
}

const std::vector<DeviceController::TransitionRecord>& DeviceController::transitionHistory() const
{
    return m_history;
}

void DeviceController::requestOpen()
{
    if (m_currentStateName == QStringLiteral("Closed") && !m_interlockCleared) {
        emit openRequestRejected();
    }
    emit requestOpenTriggered();
}

void DeviceController::requestClose()
{
    emit requestCloseTriggered();
}

void DeviceController::error()
{
    emit errorTriggered();
}

void DeviceController::reset()
{
    emit resetTriggered();
}

void DeviceController::setInterlockCleared(bool cleared)
{
    m_interlockCleared = cleared;
}

void DeviceController::buildStateMachine()
{
    m_machine = new QStateMachine(this);

    // Idle, Opening, Open, Closing and Closed are grouped under one "operational"
    // parent state purely so that the error() -> Fault transition can be added ONCE
    // on the parent instead of being repeated on every child state. Fault sits
    // outside that group as its own top-level state.
    m_operational = new QState(m_machine);
    m_operational->setChildMode(QState::ExclusiveStates);

    m_idleState = new QState(m_operational);
    m_openingState = new QState(m_operational);
    m_openState = new QState(m_operational);
    m_closingState = new QState(m_operational);
    m_closedState = new QState(m_operational);
    m_faultState = new QState(m_machine);

    m_operational->setInitialState(m_idleState);
    m_machine->setInitialState(m_operational);

    registerState(m_idleState, QStringLiteral("Idle"));
    registerState(m_openingState, QStringLiteral("Opening"));
    registerState(m_openState, QStringLiteral("Open"));
    registerState(m_closingState, QStringLiteral("Closing"));
    registerState(m_closedState, QStringLiteral("Closed"));
    registerState(m_faultState, QStringLiteral("Fault"));

    // Idle -> Opening is unconditional: there is nothing to interlock against before
    // the device has ever been closed.
    m_idleState->addTransition(this, &DeviceController::requestOpenTriggered, m_openingState);

    // Closed -> Opening is the guarded transition: this is the pattern the
    // industrial-automation showcase reuses for its own interlocks. The guard is a
    // predicate over the exact same m_interlockCleared flag that
    // setInterlockCleared() controls.
    auto* guardedOpen = new GuardedSignalTransition(
        this, &DeviceController::requestOpenTriggered, [this] { return m_interlockCleared; },
        m_closedState);
    guardedOpen->setTargetState(m_openingState);

    m_openingState->addTransition(this, &DeviceController::actuationFinished, m_openState);
    m_openState->addTransition(this, &DeviceController::requestCloseTriggered, m_closingState);
    m_closingState->addTransition(this, &DeviceController::actuationFinished, m_closedState);

    // error() is reachable from any operational state (Idle, Opening, Open, Closing,
    // Closed). Adding the transition once on the shared parent state covers all of
    // them without repeating it five times.
    m_operational->addTransition(this, &DeviceController::errorTriggered, m_faultState);

    // reset() from Fault returns to the operational group, which re-enters its
    // initial child state, Idle.
    m_faultState->addTransition(this, &DeviceController::resetTriggered, m_operational);

    connect(m_openingState, &QState::entered, this, &DeviceController::simulateActuation);
    connect(m_closingState, &QState::entered, this, &DeviceController::simulateActuation);

    m_machine->start();
}

void DeviceController::simulateActuation()
{
    QTimer::singleShot(kSimulatedActuationMs, this, &DeviceController::actuationFinished);
}

void DeviceController::registerState(QState* state, const QString& name)
{
    connect(state, &QState::entered, this, [this, name] { recordTransition(name); });
}

void DeviceController::recordTransition(const QString& toState)
{
    m_history.push_back({m_currentStateName, toState, QDateTime::currentDateTime()});
    m_currentStateName = toState;
    emit stateChanged(toState);
}

} // namespace qttutorial::state_machine
