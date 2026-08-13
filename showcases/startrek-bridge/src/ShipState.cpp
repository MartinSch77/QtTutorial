// SPDX-License-Identifier: MIT
#include "ShipState.h"

#include <QDateTime>

#include <cmath>

namespace qttutorial::startrek_bridge {

ShipState::ShipState()
{
    seedDeckMachines();
}

void ShipState::seedDeckMachines()
{
    // name, nominalLow, criticalLow, extra life-support loss (percentage
    // points) applied when power is diverted away from this deck under
    // higher alert levels. Command/medical decks are prioritized; cargo and
    // shuttle bays are the first to feel a pinch.
    const struct { const char* name; double nominalLow; double criticalLow; double extraLoss; } decks[] = {
        {"Bridge", 55.0, 30.0, 0.0},
        {"Engineering", 55.0, 30.0, 4.0},
        {"Medical Bay", 55.0, 30.0, 2.0},
        {"Crew Quarters", 55.0, 30.0, 12.0},
        {"Cargo Bay", 55.0, 30.0, 20.0},
        {"Shuttle Bay", 55.0, 30.0, 26.0},
    };

    m_deckMachines.clear();
    for (const auto& deck : decks) {
        auto machine = std::make_unique<DeckLifeSupportMachine>(QString::fromLatin1(deck.name), deck.nominalLow,
                                                                  deck.criticalLow);
        m_deckMachines.push_back(std::move(machine));
    }
    m_deckExtraLossPercent.clear();
    for (const auto& deck : decks) {
        m_deckExtraLossPercent.push_back(deck.extraLoss);
    }
}

void ShipState::startDeckMachines()
{
    for (auto& machine : m_deckMachines) {
        machine->start();
    }
}

void ShipState::advance(double dtSeconds)
{
    m_elapsedSeconds += dtSeconds;

    m_navigation.advance(dtSeconds, m_alertLevel);
    m_engineering.advance(dtSeconds, m_alertLevel);
    m_tactical.advance(dtSeconds, m_alertLevel);

    const double powerDraw = m_engineering.powerOutputPercent();
    for (std::size_t i = 0; i < m_deckMachines.size(); ++i) {
        const double phase = static_cast<double>(i) * 1.3;
        const double wobble = 9.0 * std::sin(m_elapsedSeconds * 0.15 + phase);
        const double deckValue = 95.0 - (powerDraw - 60.0) * 0.6 - m_deckExtraLossPercent[i] + wobble;
        m_deckMachines[i]->updateValue(deckValue);
    }
}

QString ShipState::stardate() const
{
    const QDateTime now = QDateTime::currentDateTime();
    const int dayOfYear = now.date().dayOfYear();
    const double fractionOfDay = now.time().msecsSinceStartOfDay() / 86400000.0;
    // An invented, Star-Trek-flavoured numeric date readout: not a
    // reproduction of any specific in-universe stardate formula, just a
    // steadily-increasing decimal number that reads like one.
    const double stardateValue = (now.date().year() - 2000) * 1000.0 + (dayOfYear + fractionOfDay) * (1000.0 / 365.0);
    return QString::number(stardateValue, 'f', 1);
}

std::vector<DeckStatus> ShipState::deckStatuses() const
{
    std::vector<DeckStatus> statuses;
    statuses.reserve(m_deckMachines.size());
    for (const auto& machine : m_deckMachines) {
        statuses.push_back({machine->deckName(), machine->currentStateName()});
    }
    return statuses;
}

} // namespace qttutorial::startrek_bridge
