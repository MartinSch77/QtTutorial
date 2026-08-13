// SPDX-License-Identifier: MIT
#include "BridgeSimulator.h"

#include <QVariantMap>

namespace qttutorial::startrek_bridge {

namespace {
constexpr int kTickIntervalMs = 150;
}

BridgeSimulator::BridgeSimulator(QObject* parent)
    : QObject(parent)
{
    m_state.startDeckMachines();

    connect(&m_timer, &QTimer::timeout, this, &BridgeSimulator::tick);
    m_timer.start(kTickIntervalMs);
}

void BridgeSimulator::setAlertLevel(int levelIndex)
{
    const auto level = static_cast<AlertLevel>(levelIndex);
    if (level == m_state.alertLevel()) {
        return;
    }
    m_state.setAlertLevel(level);
    emit alertLevelChanged();
    emit dataChanged();
}

void BridgeSimulator::tick()
{
    m_state.advance(kTickIntervalMs / 1000.0);
    emit dataChanged();
}

QVariantList BridgeSimulator::navContacts() const
{
    QVariantList list;
    for (const NavContact& contact : m_state.navigation().contacts()) {
        QVariantMap map;
        map[QStringLiteral("id")] = contact.id;
        map[QStringLiteral("label")] = contact.label;
        map[QStringLiteral("xKm")] = contact.xKm;
        map[QStringLiteral("yKm")] = contact.yKm;
        map[QStringLiteral("headingDeg")] = contact.headingDeg;
        list.push_back(map);
    }
    return list;
}

QVariantList BridgeSimulator::deckStatuses() const
{
    QVariantList list;
    for (const DeckStatus& status : m_state.deckStatuses()) {
        QVariantMap map;
        map[QStringLiteral("name")] = status.name;
        map[QStringLiteral("state")] = status.stateName;
        list.push_back(map);
    }
    return list;
}

QVariantList BridgeSimulator::sensorContacts() const
{
    QVariantList list;
    for (const SensorContact& contact : m_state.tactical().contacts()) {
        QVariantMap map;
        map[QStringLiteral("id")] = contact.id;
        map[QStringLiteral("label")] = contact.label;
        map[QStringLiteral("distanceKm")] = contact.distanceKm;
        map[QStringLiteral("bearingDeg")] = contact.bearingDeg;
        list.push_back(map);
    }
    return list;
}

} // namespace qttutorial::startrek_bridge
