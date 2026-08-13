// SPDX-License-Identifier: MIT
#pragma once

#include <QDateTime>
#include <QObject>
#include <QString>

#include <vector>

class QState;
class QStateMachine;

namespace qttutorial::state_machine {

// Models the lifecycle of an actuated device (e.g. a valve or a door actuator) as a
// hierarchical QStateMachine with the states: Idle, Opening, Open, Closing, Closed,
// Fault. This class is deliberately generic: the state names and the guard pattern
// used for Closed -> Opening are meant to be a reusable reference for other examples
// (e.g. an industrial-automation showcase) that need the same shape.
class DeviceController : public QObject {
    Q_OBJECT
public:
    struct TransitionRecord {
        QString fromState;
        QString toState;
        QDateTime timestamp;
    };

    explicit DeviceController(QObject* parent = nullptr);

    QString currentState() const;
    bool interlockCleared() const;
    const std::vector<TransitionRecord>& transitionHistory() const;

public slots:
    void requestOpen();
    void requestClose();
    void error();
    void reset();
    void setInterlockCleared(bool cleared);

signals:
    void stateChanged(QString stateName);

    // Emitted synchronously from requestOpen() when the caller can already tell,
    // from the same interlock flag the guard itself checks, that the Closed ->
    // Opening transition is about to be rejected. This is informational only: the
    // state machine's own GuardedSignalTransition remains the single source of
    // truth for whether the transition actually happens.
    void openRequestRejected();

    // Internal signals consumed only by the state machine's own transitions.
    void requestOpenTriggered();
    void requestCloseTriggered();
    void errorTriggered();
    void resetTriggered();
    void actuationFinished();

private:
    void buildStateMachine();
    void registerState(QState* state, const QString& name);
    void recordTransition(const QString& toState);
    void simulateActuation();

    QStateMachine* m_machine;
    QState* m_operational;
    QState* m_idleState;
    QState* m_openingState;
    QState* m_openState;
    QState* m_closingState;
    QState* m_closedState;
    QState* m_faultState;

    bool m_interlockCleared = false;
    QString m_currentStateName;
    std::vector<TransitionRecord> m_history;
};

} // namespace qttutorial::state_machine
