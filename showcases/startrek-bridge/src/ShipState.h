// SPDX-License-Identifier: MIT
#pragma once

#include "AlertLevel.h"
#include "DeckLifeSupportMachine.h"
#include "EngineeringSimulator.h"
#include "NavigationSimulator.h"
#include "TacticalSimulator.h"

#include <QString>

#include <memory>
#include <vector>

namespace qttutorial::startrek_bridge {

// A single deck's life-support status, as republished from its
// DeckLifeSupportMachine for whoever is displaying the deck grid.
struct DeckStatus {
    QString name;
    QString stateName;
};

// The pure, testable simulated ship model shared by every bridge station:
// the ship-wide AlertLevel plus the Navigation/Engineering/Tactical
// simulators and a handful of DeckLifeSupportMachines. Has no Qt Quick or
// QML dependency at all - a QML-facing facade (BridgeSimulator) owns one of
// these and republishes it as Q_PROPERTYs. advance() drives every
// subsystem forward by one simulated time step using the current
// AlertLevel, so e.g. going to Red alert visibly changes engineering power
// draw, tactical shield strength and deck life-support margins together,
// consistently, rather than each station inventing its own notion of
// "red alert."
class ShipState {
public:
    ShipState();

    void advance(double dtSeconds);
    void startDeckMachines();

    void setAlertLevel(AlertLevel level) { m_alertLevel = level; }
    [[nodiscard]] AlertLevel alertLevel() const { return m_alertLevel; }

    [[nodiscard]] QString shipName() const { return m_shipName; }
    [[nodiscard]] QString stardate() const;

    [[nodiscard]] NavigationSimulator& navigation() { return m_navigation; }
    [[nodiscard]] const NavigationSimulator& navigation() const { return m_navigation; }
    [[nodiscard]] EngineeringSimulator& engineering() { return m_engineering; }
    [[nodiscard]] const EngineeringSimulator& engineering() const { return m_engineering; }
    [[nodiscard]] TacticalSimulator& tactical() { return m_tactical; }
    [[nodiscard]] const TacticalSimulator& tactical() const { return m_tactical; }

    [[nodiscard]] std::vector<DeckStatus> deckStatuses() const;

private:
    void seedDeckMachines();

    QString m_shipName = QStringLiteral("USS Horizon");
    AlertLevel m_alertLevel = AlertLevel::Green;
    NavigationSimulator m_navigation;
    EngineeringSimulator m_engineering;
    TacticalSimulator m_tactical;
    std::vector<std::unique_ptr<DeckLifeSupportMachine>> m_deckMachines;
    std::vector<double> m_deckExtraLossPercent;
    double m_elapsedSeconds = 0.0;
};

} // namespace qttutorial::startrek_bridge
