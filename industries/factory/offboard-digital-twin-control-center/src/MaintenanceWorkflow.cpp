// SPDX-License-Identifier: GPL-3.0-or-later
#include "MaintenanceWorkflow.h"

namespace qttutorial::offboard_digital_twin {

namespace {
constexpr int kStepDurationMs = 1400; // ~1.4s per step * 4 steps => fits comfortably in the ~90s demo
constexpr int kProgressTickMs = 70;
}

MaintenanceWorkflow::MaintenanceWorkflow(int machineId, QObject *parent)
    : QObject(parent)
    , m_idleState(new QState(&m_machine))
    , m_diagnosingState(new QState(&m_machine))
    , m_stoppingState(new QState(&m_machine))
    , m_replacingState(new QState(&m_machine))
    , m_restartingState(new QState(&m_machine))
    , m_completeState(new QState(&m_machine))
    , m_errorState(new QState(&m_machine))
    , m_machineId(machineId)
{
    // Linear happy path. "stepSucceeded" is emitted by advanceProgress() once
    // a step's timer-driven progress reaches 1.0; each state also listens for
    // the shared error/cancel signals so the transitions read the same way a
    // TaskTree Group's sequential+error-handling structure would.
    m_idleState->addTransition(this, &MaintenanceWorkflow::stepStarted, m_diagnosingState);
    m_diagnosingState->addTransition(this, &MaintenanceWorkflow::finished, m_stoppingState);
    m_stoppingState->addTransition(this, &MaintenanceWorkflow::finished, m_replacingState);
    m_replacingState->addTransition(this, &MaintenanceWorkflow::finished, m_restartingState);
    m_restartingState->addTransition(this, &MaintenanceWorkflow::finished, m_completeState);

    for (QState *active : {m_diagnosingState, m_stoppingState, m_replacingState, m_restartingState}) {
        active->addTransition(this, &MaintenanceWorkflow::cancelled, m_idleState);
        active->addTransition(this, &MaintenanceWorkflow::stepFailed, m_errorState);
    }
    m_errorState->addTransition(this, &MaintenanceWorkflow::cancelled, m_idleState);
    m_errorState->addTransition(this, &MaintenanceWorkflow::stepStarted, m_diagnosingState);

    m_machine.setInitialState(m_idleState);
    m_machine.start();

    m_stepTimer.setInterval(kProgressTickMs);
    connect(&m_stepTimer, &QTimer::timeout, this, &MaintenanceWorkflow::advanceProgress);
}

QString MaintenanceWorkflow::stepName() const
{
    switch (m_step) {
    case Idle: return QStringLiteral("Idle");
    case Diagnosing: return QStringLiteral("Diagnosing");
    case Stopping: return QStringLiteral("Stopping machine");
    case ReplacingPart: return QStringLiteral("Replacing part");
    case Restarting: return QStringLiteral("Restarting");
    case Complete: return QStringLiteral("Complete");
    case Error: return QStringLiteral("Error");
    }
    return {};
}

bool MaintenanceWorkflow::isRunning() const
{
    return m_step != Idle && m_step != Complete && m_step != Error;
}

void MaintenanceWorkflow::start()
{
    if (isRunning())
        return;
    m_errorMessage.clear();
    enterStep(Diagnosing);
    emit stepStarted(Diagnosing);
    emit errorChanged(); // reset any prior error UI state (inError is now false)
}

void MaintenanceWorkflow::cancel()
{
    m_stepTimer.stop();
    m_progress = 0.0;
    enterStep(Idle);
    emit cancelled();
    emit progressChanged();
}

void MaintenanceWorkflow::retry()
{
    if (m_step != Error)
        return;
    m_errorMessage.clear();
    enterStep(m_failedStep);
    emit stepStarted(m_failedStep);
    emit errorChanged();
}

void MaintenanceWorkflow::injectFailureOnNextStep()
{
    m_failNextStep = true;
}

void MaintenanceWorkflow::enterStep(Step step)
{
    m_step = step;
    m_progress = 0.0;
    emit stepChanged();
    emit progressChanged();
    emit runningChanged();
    if (step == Diagnosing || step == Stopping || step == ReplacingPart || step == Restarting)
        m_stepTimer.start();
    else
        m_stepTimer.stop();
}

void MaintenanceWorkflow::advanceProgress()
{
    m_progress += static_cast<double>(kProgressTickMs) / kStepDurationMs;
    if (m_progress < 1.0) {
        emit progressChanged();
        return;
    }

    m_stepTimer.stop();
    m_progress = 1.0;
    emit progressChanged();

    if (m_failNextStep) {
        m_failNextStep = false;
        m_failedStep = m_step;
        m_errorMessage = QStringLiteral("%1 failed: sensor timeout on machine %2 - retry or cancel.")
                              .arg(stepName())
                              .arg(m_machineId);
        m_step = Error;
        emit stepChanged();
        emit runningChanged();
        emit errorChanged();
        emit stepFailed();
        return;
    }

    Step next = Idle;
    switch (m_step) {
    case Diagnosing: next = Stopping; break;
    case Stopping: next = ReplacingPart; break;
    case ReplacingPart: next = Restarting; break;
    case Restarting: next = Complete; break;
    default: break;
    }

    if (next == Complete) {
        m_step = Complete;
        emit stepChanged();
        emit runningChanged();
        emit finished();
        return;
    }

    emit finished(); // drives the QStateMachine's linear transition
    enterStep(next);
    emit stepStarted(next);
}

} // namespace qttutorial::offboard_digital_twin
