// SPDX-License-Identifier: MIT
#pragma once

#include "HaulCycleSimulator.h"

#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QVariantList>

namespace qttutorial::mining {

// Thin QObject/QML façade over HaulCycleSimulator: owns the timer that steps the
// simulation and republishes its HaulTruckState as Q_PROPERTYs the haul-truck
// console QML can bind to. All the actual haul-cycle logic lives in
// HaulCycleSimulator so it can be unit tested without QML or an event loop.
class HaulTruckTelemetry : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int haulStateIndex READ haulStateIndex NOTIFY telemetryChanged)
    Q_PROPERTY(QString haulStateLabel READ haulStateLabel NOTIFY telemetryChanged)
    Q_PROPERTY(double payloadTonnes READ payloadTonnes NOTIFY telemetryChanged)
    Q_PROPERTY(double ratedCapacityTonnes READ ratedCapacityTonnes CONSTANT)
    Q_PROPERTY(bool overloaded READ overloaded NOTIFY telemetryChanged)
    Q_PROPERTY(double engineTempC READ engineTempC NOTIFY telemetryChanged)
    Q_PROPERTY(double retarderTempC READ retarderTempC NOTIFY telemetryChanged)
    Q_PROPERTY(double speedKph READ speedKph NOTIFY telemetryChanged)
    Q_PROPERTY(double fuelLtrPerHour READ fuelLtrPerHour NOTIFY telemetryChanged)
    Q_PROPERTY(double speedExpectedMinKph READ speedExpectedMinKph NOTIFY telemetryChanged)
    Q_PROPERTY(double speedExpectedMaxKph READ speedExpectedMaxKph NOTIFY telemetryChanged)
    Q_PROPERTY(bool speedOutOfRange READ speedOutOfRange NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList tyrePressuresKPa READ tyrePressuresKPa NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList tyreTempsC READ tyreTempsC NOTIFY telemetryChanged)
public:
    explicit HaulTruckTelemetry(QObject* parent = nullptr);

    [[nodiscard]] int haulStateIndex() const { return static_cast<int>(m_state.haulState); }
    [[nodiscard]] QString haulStateLabel() const;
    [[nodiscard]] double payloadTonnes() const { return m_state.payloadTonnes; }
    [[nodiscard]] double ratedCapacityTonnes() const { return HaulCycleSimulator::kRatedCapacityTonnes; }
    [[nodiscard]] bool overloaded() const { return m_state.overloaded; }
    [[nodiscard]] double engineTempC() const { return m_state.engineTempC; }
    [[nodiscard]] double retarderTempC() const { return m_state.retarderTempC; }
    [[nodiscard]] double speedKph() const { return m_state.speedKph; }
    [[nodiscard]] double fuelLtrPerHour() const { return m_state.fuelLtrPerHour; }
    [[nodiscard]] double speedExpectedMinKph() const;
    [[nodiscard]] double speedExpectedMaxKph() const;
    [[nodiscard]] bool speedOutOfRange() const;
    [[nodiscard]] QVariantList tyrePressuresKPa() const;
    [[nodiscard]] QVariantList tyreTempsC() const;

signals:
    void telemetryChanged();

private:
    void tick();

    HaulCycleSimulator m_simulator;
    HaulTruckState m_state;
    QTimer m_timer;
};

} // namespace qttutorial::mining
