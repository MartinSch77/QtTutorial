// SPDX-License-Identifier: MIT
#include "SubsystemHealthMonitor.h"

namespace qttutorial::defence {

SubsystemHealthMonitor::SubsystemHealthMonitor(double nominalLow, double nominalHigh, double criticalLow,
                                                double criticalHigh, QObject* parent)
    : QObject(parent)
    , m_nominalLow(nominalLow)
    , m_nominalHigh(nominalHigh)
    , m_criticalLow(criticalLow)
    , m_criticalHigh(criticalHigh)
{
}

SubsystemHealthMonitor::Classification SubsystemHealthMonitor::classify(double value) const
{
    if (value < m_criticalLow || value > m_criticalHigh) {
        return Classification::Critical;
    }
    if (value < m_nominalLow || value > m_nominalHigh) {
        return Classification::Caution;
    }
    return Classification::Nominal;
}

void SubsystemHealthMonitor::evaluate(double value)
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

} // namespace qttutorial::defence
