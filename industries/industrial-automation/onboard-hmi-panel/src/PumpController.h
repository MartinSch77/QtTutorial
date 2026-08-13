// SPDX-License-Identifier: MIT
#pragma once

#include <QSignalTransition>
#include <QState>
#include <QStateMachine>

#include <functional>

namespace qttutorial::industrial_hmi {

// A QSignalTransition that additionally requires a guard predicate to hold,
// so "start" only actually moves Idle -> Running when the process is in a
// safe condition (not already overfilled) rather than unconditionally.
class GuardedSignalTransition : public QSignalTransition {
public:
    GuardedSignalTransition(QObject* sender, const char* signal, std::function<bool()> guard);

protected:
    bool eventTest(QEvent* event) override;

private:
    std::function<bool()> m_guard;
};

// Models a pump lifecycle: Idle -> Running -> Idle, with any state able to
// fall into Fault, and Fault only clearable via an explicit reset(). Mirrors
// the kind of guarded QStateMachine lifecycle used elsewhere in this
// tutorial for actuators, applied here to a pump.
class PumpController : public QObject {
    Q_OBJECT
    Q_PROPERTY(int stateIndex READ stateIndex NOTIFY stateChanged)
public:
    enum class State { Idle, Running, Fault };

    explicit PumpController(QObject* parent = nullptr);

    [[nodiscard]] State state() const { return m_state; }
    [[nodiscard]] int stateIndex() const { return static_cast<int>(m_state); }

    // Guard consulted by the Idle -> Running transition: start() is refused
    // (silently ignored, machine stays Idle) when this returns false.
    void setStartGuard(std::function<bool()> guard);

public slots:
    void start();
    void stop();
    void raiseFault();
    void reset();

signals:
    void started();
    void stopped();
    void faultRaised();
    void resetRequested();
    void stateChanged(State state);

private:
    void enterState(State state);

    QStateMachine m_machine;
    QState* m_idle;
    QState* m_running;
    QState* m_fault;
    State m_state = State::Idle;
    std::function<bool()> m_startGuard = [] { return true; };
};

} // namespace qttutorial::industrial_hmi
