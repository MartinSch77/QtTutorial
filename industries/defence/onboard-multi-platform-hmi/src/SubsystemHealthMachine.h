// SPDX-License-Identifier: MIT
#pragma once

#include "SubsystemHealthMonitor.h"

#include <QState>
#include <QStateMachine>
#include <QString>

namespace qttutorial::defence {

// A small, real Qt State Machine (Qt6::StateMachine module): three QState
// objects (Nominal/Caution/Critical) wired to a SubsystemHealthMonitor's
// edge-triggered signals via QState::addTransition. Drives each subsystem's
// health-grid cell on the vehicle crew display.
class SubsystemHealthMachine : public QObject {
    Q_OBJECT
public:
    SubsystemHealthMachine(QString subsystemName, double nominalLow, double nominalHigh, double criticalLow,
                            double criticalHigh, QObject* parent = nullptr);

    void start();
    void updateValue(double value);

    [[nodiscard]] QString subsystemName() const { return m_subsystemName; }
    [[nodiscard]] QString currentStateName() const { return m_currentStateName; }

signals:
    void healthChanged(const QString& subsystemName, const QString& stateName);

private:
    QString m_subsystemName;
    QString m_currentStateName = QStringLiteral("Nominal");
    SubsystemHealthMonitor m_monitor;
    QStateMachine m_machine;
    QState m_nominalState;
    QState m_cautionState;
    QState m_criticalState;
};

} // namespace qttutorial::defence
