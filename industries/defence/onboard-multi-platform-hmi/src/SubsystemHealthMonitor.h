// SPDX-License-Identifier: MIT
#pragma once

#include <QObject>

namespace qttutorial::defence {

// Classifies a single subsystem parameter into Nominal/Caution/Critical
// against fixed bands and emits a signal only when the classification
// changes, so it can drive a QStateMachine directly without re-entering the
// same state on every sample.
class SubsystemHealthMonitor : public QObject {
    Q_OBJECT
public:
    SubsystemHealthMonitor(double nominalLow, double nominalHigh, double criticalLow, double criticalHigh,
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

} // namespace qttutorial::defence
