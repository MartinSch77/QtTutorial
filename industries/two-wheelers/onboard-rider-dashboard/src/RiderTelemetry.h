// SPDX-License-Identifier: MIT
#pragma once

#include "RideCycleSimulator.h"

#include <QObject>
#include <QQmlEngine>
#include <QTimer>

namespace qttutorial::two_wheelers {

// Thin QObject/QML façade over RideCycleSimulator: owns the timer that steps the
// simulation and republishes its RideState as Q_PROPERTYs the rider dashboard QML
// can bind to. All the actual ride-cycle logic lives in RideCycleSimulator so it
// can be unit tested without QML or an event loop.
class RiderTelemetry : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(double speedKph READ speedKph NOTIFY telemetryChanged)
    Q_PROPERTY(double rpm READ rpm NOTIFY telemetryChanged)
    Q_PROPERTY(QString gearLabel READ gearLabel NOTIFY telemetryChanged)
    Q_PROPERTY(double leanAngleDeg READ leanAngleDeg NOTIFY telemetryChanged)
    Q_PROPERTY(double frontTyrePressureBar READ frontTyrePressureBar NOTIFY telemetryChanged)
    Q_PROPERTY(double rearTyrePressureBar READ rearTyrePressureBar NOTIFY telemetryChanged)
    Q_PROPERTY(double frontTyreTempC READ frontTyreTempC NOTIFY telemetryChanged)
    Q_PROPERTY(double rearTyreTempC READ rearTyreTempC NOTIFY telemetryChanged)
    Q_PROPERTY(QString ridingModeLabel READ ridingModeLabel NOTIFY telemetryChanged)
    Q_PROPERTY(int ridingModeIndex READ ridingModeIndex NOTIFY telemetryChanged)
    Q_PROPERTY(double fuelLitres READ fuelLitres NOTIFY telemetryChanged)
    Q_PROPERTY(double fuelPercent READ fuelPercent NOTIFY telemetryChanged)
public:
    explicit RiderTelemetry(QObject* parent = nullptr);

    [[nodiscard]] double speedKph() const { return m_state.speedKph; }
    [[nodiscard]] double rpm() const { return m_state.rpm; }
    [[nodiscard]] QString gearLabel() const;
    [[nodiscard]] double leanAngleDeg() const { return m_state.leanAngleDeg; }
    [[nodiscard]] double frontTyrePressureBar() const { return m_state.frontTyrePressureBar; }
    [[nodiscard]] double rearTyrePressureBar() const { return m_state.rearTyrePressureBar; }
    [[nodiscard]] double frontTyreTempC() const { return m_state.frontTyreTempC; }
    [[nodiscard]] double rearTyreTempC() const { return m_state.rearTyreTempC; }
    [[nodiscard]] QString ridingModeLabel() const;
    [[nodiscard]] int ridingModeIndex() const { return static_cast<int>(m_state.mode); }
    [[nodiscard]] double fuelLitres() const { return m_state.fuelLitres; }
    [[nodiscard]] double fuelPercent() const;

    // Called from RidingModeSelector.qml when the rider taps a mode button.
    // modeIndex follows the RidingMode enum order: 0=Rain, 1=Road, 2=Sport, 3=Race.
    Q_INVOKABLE void setRidingMode(int modeIndex);

signals:
    void telemetryChanged();

private:
    void tick();

    RideCycleSimulator m_simulator;
    RideState m_state;
    QTimer m_timer;
};

} // namespace qttutorial::two_wheelers
