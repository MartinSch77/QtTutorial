// SPDX-License-Identifier: MIT
#pragma once

#include <QObject>
#include <QString>

namespace qttutorial::space {

// Classifies a single parameter into Nominal/Caution/Critical against fixed
// bands and emits a signal *only when the classification changes*. This edge
// -triggered behaviour is what makes it safe to drive a QStateMachine directly
// from these signals (see SubsystemHealthMachine) without the machine
// re-entering the same state on every sample.
class SubsystemMonitor : public QObject {
    Q_OBJECT
public:
    SubsystemMonitor(double nominalLow, double nominalHigh, double criticalLow, double criticalHigh,
                      QObject* parent = nullptr);

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
    double m_nominalHigh;
    double m_criticalLow;
    double m_criticalHigh;
    Classification m_last = Classification::Nominal;
    bool m_hasEmitted = false;
};

} // namespace qttutorial::space
