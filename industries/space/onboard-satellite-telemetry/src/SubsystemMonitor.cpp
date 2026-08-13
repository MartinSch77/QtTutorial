// SPDX-License-Identifier: MIT
#include "SubsystemMonitor.h"

namespace qttutorial::space {

SubsystemMonitor::SubsystemMonitor(double nominalLow, double nominalHigh, double criticalLow, double criticalHigh,
                                    QObject* parent)
    : QObject(parent)
    , m_nominalLow(nominalLow)
    , m_nominalHigh(nominalHigh)
    , m_criticalLow(criticalLow)
    , m_criticalHigh(criticalHigh)
{
}

SubsystemMonitor::Classification SubsystemMonitor::classify(double value) const
{
    if (value < m_criticalLow || value > m_criticalHigh) {
        return Classification::Critical;
    }
    if (value < m_nominalLow || value > m_nominalHigh) {
        return Classification::Caution;
    }
    return Classification::Nominal;
}

void SubsystemMonitor::evaluate(double value)
{
    const Classification classification = classify(value);
    if (m_hasEmitted && classification == m_last) {
        return;
    }
    m_last = classification;
    m_hasEmitted = true;

    switch (classification) {
    case Classification::Nominal:
        emit nominalDetected();
        break;
    case Classification::Caution:
        emit cautionDetected();
        break;
    case Classification::Critical:
        emit criticalDetected();
        break;
    }
}

} // namespace qttutorial::space
