// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QState>
#include <QStateMachine>
#include <QString>
#include <QTimer>

namespace qttutorial::offboard_digital_twin {

// Step 5 of the demo scenario: "Diagnose -> Stop machine -> Replace part ->
// Restart", with per-step progress and working cancel/retry/error-recovery.
//
// The brief asked us to attempt Qt6::TaskTree (QtTaskTree::Group/For/When)
// for this workflow first. qt_documentation_search confirmed QtTaskTree is a
// real module (find_package(Qt6 REQUIRED COMPONENTS TaskTree)) but it is
// "Since: Qt 6.11, Status: Technical Preview" - this repository's local Qt
// baseline is 6.4.2 (see root CMakeLists.txt's `find_package(Qt6 6.4
// REQUIRED ...)`), so Qt6::TaskTree does not exist here and could not be
// built/verified in this sandbox. Per the brief's fallback instruction, this
// class implements the identical workflow with a hand-rolled QStateMachine
// instead - the public API and observable behavior (per-step progress,
// cancel, retry, error-recovery) are unaffected by that substitution.
class MaintenanceWorkflow : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString stepName READ stepName NOTIFY stepChanged)
    Q_PROPERTY(int stepIndex READ stepIndex NOTIFY stepChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(bool inError READ isInError NOTIFY errorChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorChanged)

public:
    enum Step { Idle, Diagnosing, Stopping, ReplacingPart, Restarting, Complete, Error };
    Q_ENUM(Step)

    explicit MaintenanceWorkflow(int machineId, QObject *parent = nullptr);

    [[nodiscard]] QString stepName() const;
    [[nodiscard]] int stepIndex() const { return static_cast<int>(m_step); }
    [[nodiscard]] double progress() const { return m_progress; }
    [[nodiscard]] bool isRunning() const;
    [[nodiscard]] bool isInError() const { return m_step == Error; }
    [[nodiscard]] QString errorMessage() const { return m_errorMessage; }
    [[nodiscard]] int machineId() const { return m_machineId; }

public slots:
    void start();
    void cancel();
    void retry();
    // Test/demo hook: force the currently active step to fail instead of
    // succeeding, to exercise the error-recovery path deterministically.
    void injectFailureOnNextStep();

signals:
    void stepChanged();
    void progressChanged();
    void runningChanged();
    void errorChanged();
    void finished();
    void cancelled();
    // Emitted whenever a concrete step *starts*, so the QML side can drive
    // the matching 3D-scene animation (e.g. fade to transparent on Stopping).
    void stepStarted(Step step);
    // Distinct from errorChanged() (the Q_PROPERTY notify signal, emitted at
    // several points including error *recovery*): this one exists purely as
    // the QStateMachine transition trigger into m_errorState, so it must only
    // ever fire on an actual failure.
    void stepFailed();

private:
    void enterStep(Step step);
    void advanceProgress();

    QStateMachine m_machine;
    QState *m_idleState;
    QState *m_diagnosingState;
    QState *m_stoppingState;
    QState *m_replacingState;
    QState *m_restartingState;
    QState *m_completeState;
    QState *m_errorState;

    QTimer m_stepTimer;
    Step m_step = Idle;
    double m_progress = 0.0;
    int m_machineId;
    QString m_errorMessage;
    bool m_failNextStep = false;
    Step m_failedStep = Idle;
};

} // namespace qttutorial::offboard_digital_twin
