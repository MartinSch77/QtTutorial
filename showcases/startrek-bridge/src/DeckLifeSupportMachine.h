// SPDX-License-Identifier: MIT
#pragma once

#include "DeckLifeSupportMonitor.h"

#include <QState>
#include <QStateMachine>
#include <QString>

namespace qttutorial::startrek_bridge {

// A small, real Qt State Machine (Qt6::StateMachine module) backing one
// deck's life-support status cell: three QState objects
// (Nominal/Caution/Critical) wired to a DeckLifeSupportMonitor's
// edge-triggered signals via QState::addTransition. This is what makes deck
// status changes believable transitions rather than random per-frame
// flicker.
class DeckLifeSupportMachine : public QObject {
    Q_OBJECT
public:
    DeckLifeSupportMachine(QString deckName, double nominalLow, double criticalLow, QObject* parent = nullptr);

    void start();
    void updateValue(double value);

    [[nodiscard]] QString deckName() const { return m_deckName; }
    [[nodiscard]] QString currentStateName() const { return m_currentStateName; }

signals:
    void statusChanged(const QString& deckName, const QString& stateName);

private:
    QString m_deckName;
    QString m_currentStateName = QStringLiteral("Nominal");
    DeckLifeSupportMonitor m_monitor;
    QStateMachine m_machine;
    QState m_nominalState;
    QState m_cautionState;
    QState m_criticalState;
};

} // namespace qttutorial::startrek_bridge
