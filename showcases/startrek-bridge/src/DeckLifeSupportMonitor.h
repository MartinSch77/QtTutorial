// SPDX-License-Identifier: MIT
#pragma once

#include <QObject>

namespace qttutorial::startrek_bridge {

// Classifies a single deck's composite life-support reading (0-100, higher
// is healthier) into Nominal/Caution/Critical against fixed bands and emits
// a signal only when the classification changes, so it can drive a
// QStateMachine directly without re-entering the same state every sample.
class DeckLifeSupportMonitor : public QObject {
    Q_OBJECT
public:
    DeckLifeSupportMonitor(double nominalLow, double criticalLow, QObject* parent = nullptr);

public slots:
    void evaluate(double value);

signals:
    void nominalDetected();
    void cautionDetected();
    void criticalDetected();

private:
    enum class Classification { Nominal, Caution, Critical };
    [[nodiscard]] Classification classify(double value) const;

    double m_nominalLow;
    double m_criticalLow;
    Classification m_last = Classification::Nominal;
    bool m_hasEmitted = false;
};

} // namespace qttutorial::startrek_bridge
