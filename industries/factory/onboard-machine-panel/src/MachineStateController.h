// SPDX-License-Identifier: MIT
#pragma once

#include <QState>
#include <QStateMachine>

namespace qttutorial::factory_machine_panel {

// Models the operator-visible lifecycle of a single machine with a
// QStateMachine: Idle -> Running -> Warning -> Fault -> Idle, with a Paused
// state reachable (and resumable) from Running. Warning is a "still
// producing but degraded" state that can either clear back to Running or
// escalate to Fault; Fault is a hard stop only clearable by an explicit
// operator acknowledgement.
class MachineStateController : public QObject {
    Q_OBJECT
    Q_PROPERTY(int stateIndex READ stateIndex NOTIFY stateChanged)
public:
    enum class State { Idle, Running, Paused, Warning, Fault };

    explicit MachineStateController(QObject* parent = nullptr);

    [[nodiscard]] State state() const { return m_state; }
    [[nodiscard]] int stateIndex() const { return static_cast<int>(m_state); }

public slots:
    void start();
    void stop();
    void pause();
    void raiseWarning();
    void clearWarning();
    void raiseFault();
    void acknowledge();

signals:
    void started();
    void stopped();
    void paused();
    void warningRaised();
    void warningCleared();
    void faultRaised();
    void acknowledged();
    void stateChanged(State state);

private:
    void enterState(State state);

    QStateMachine m_machine;
    QState* m_idle;
    QState* m_running;
    QState* m_paused;
    QState* m_warning;
    QState* m_fault;
    State m_state = State::Idle;
};

} // namespace qttutorial::factory_machine_panel
