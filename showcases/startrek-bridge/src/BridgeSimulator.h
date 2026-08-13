// SPDX-License-Identifier: MIT
#pragma once

#include "ShipState.h"

#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QVariantList>

namespace qttutorial::startrek_bridge {

// Thin QObject/QML facade that owns the timer driving the pure ShipState
// forward and republishes it as QML-bindable properties. Every station
// panel binds to the same BridgeSimulator instance, so setting the alert
// level from any one of them is immediately reflected everywhere else. All
// the actual simulation logic lives in ShipState and its sub-simulators, so
// it stays unit-testable without QML or a running event loop.
class BridgeSimulator : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString shipName READ shipName CONSTANT)
    Q_PROPERTY(QString stardate READ stardate NOTIFY dataChanged)
    Q_PROPERTY(int alertLevelIndex READ alertLevelIndex NOTIFY alertLevelChanged)
    Q_PROPERTY(QString alertLevelName READ alertLevelName NOTIFY alertLevelChanged)
    Q_PROPERTY(double headingDeg READ headingDeg NOTIFY dataChanged)
    Q_PROPERTY(double warpFactor READ warpFactor NOTIFY dataChanged)
    Q_PROPERTY(double impulsePercent READ impulsePercent NOTIFY dataChanged)
    Q_PROPERTY(QVariantList navContacts READ navContacts NOTIFY dataChanged)
    Q_PROPERTY(double navAreaRadiusKm READ navAreaRadiusKm CONSTANT)
    Q_PROPERTY(double powerOutputPercent READ powerOutputPercent NOTIFY dataChanged)
    Q_PROPERTY(double hullIntegrityPercent READ hullIntegrityPercent NOTIFY dataChanged)
    Q_PROPERTY(double internalTempC READ internalTempC NOTIFY dataChanged)
    Q_PROPERTY(QVariantList deckStatuses READ deckStatuses NOTIFY dataChanged)
    Q_PROPERTY(double externalRadiationLevel READ externalRadiationLevel NOTIFY dataChanged)
    Q_PROPERTY(double hullStressLevel READ hullStressLevel NOTIFY dataChanged)
    Q_PROPERTY(double shieldStrengthPercent READ shieldStrengthPercent NOTIFY dataChanged)
    Q_PROPERTY(QVariantList sensorContacts READ sensorContacts NOTIFY dataChanged)
public:
    explicit BridgeSimulator(QObject* parent = nullptr);

    [[nodiscard]] QString shipName() const { return m_state.shipName(); }
    [[nodiscard]] QString stardate() const { return m_state.stardate(); }
    [[nodiscard]] int alertLevelIndex() const { return static_cast<int>(m_state.alertLevel()); }
    [[nodiscard]] QString alertLevelName() const { return qttutorial::startrek_bridge::alertLevelName(m_state.alertLevel()); }
    [[nodiscard]] double headingDeg() const { return m_state.navigation().headingDeg(); }
    [[nodiscard]] double warpFactor() const { return m_state.navigation().warpFactor(); }
    [[nodiscard]] double impulsePercent() const { return m_state.navigation().impulsePercent(); }
    [[nodiscard]] QVariantList navContacts() const;
    [[nodiscard]] double navAreaRadiusKm() const { return m_state.navigation().areaRadiusKm(); }
    [[nodiscard]] double powerOutputPercent() const { return m_state.engineering().powerOutputPercent(); }
    [[nodiscard]] double hullIntegrityPercent() const { return m_state.engineering().hullIntegrityPercent(); }
    [[nodiscard]] double internalTempC() const { return m_state.engineering().internalTempC(); }
    [[nodiscard]] QVariantList deckStatuses() const;
    [[nodiscard]] double externalRadiationLevel() const { return m_state.tactical().externalRadiationLevel(); }
    [[nodiscard]] double hullStressLevel() const { return m_state.tactical().hullStressLevel(); }
    [[nodiscard]] double shieldStrengthPercent() const { return m_state.tactical().shieldStrengthPercent(); }
    [[nodiscard]] QVariantList sensorContacts() const;

public slots:
    void setAlertLevel(int levelIndex);

signals:
    void dataChanged();
    void alertLevelChanged();

private:
    void tick();

    ShipState m_state;
    QTimer m_timer;
};

} // namespace qttutorial::startrek_bridge
