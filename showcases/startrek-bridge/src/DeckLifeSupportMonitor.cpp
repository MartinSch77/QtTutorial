// SPDX-License-Identifier: MIT
#include "DeckLifeSupportMonitor.h"

namespace qttutorial::startrek_bridge {

DeckLifeSupportMonitor::DeckLifeSupportMonitor(double nominalLow, double criticalLow, QObject* parent)
    : QObject(parent)
    , m_nominalLow(nominalLow)
    , m_criticalLow(criticalLow)
{
}

DeckLifeSupportMonitor::Classification DeckLifeSupportMonitor::classify(double value) const
{
    if (value < m_criticalLow) {
        return Classification::Critical;
    }
    if (value < m_nominalLow) {
        return Classification::Caution;
    }
    return Classification::Nominal;
}

void DeckLifeSupportMonitor::evaluate(double value)
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

} // namespace qttutorial::startrek_bridge
