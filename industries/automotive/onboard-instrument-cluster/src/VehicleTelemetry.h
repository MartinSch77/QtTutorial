// SPDX-License-Identifier: MIT
#pragma once

#include "DriveCycleSimulator.h"

#include <QColor>
#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QVariantList>

namespace qttutorial::automotive {

// Thin QObject/QML façade over DriveCycleSimulator: owns the timer that steps the
// simulation and republishes its VehicleState as Q_PROPERTYs the instrument-cluster
// QML can bind to. All the actual drivetrain logic lives in DriveCycleSimulator so
// it can be unit tested without QML or an event loop.
class VehicleTelemetry : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(double speedKph READ speedKph NOTIFY telemetryChanged)
    Q_PROPERTY(double rpm READ rpm NOTIFY telemetryChanged)
    Q_PROPERTY(QString gearLabel READ gearLabel NOTIFY telemetryChanged)
    Q_PROPERTY(double fuelLevel READ fuelLevel NOTIFY telemetryChanged)
    Q_PROPERTY(bool leftTurnSignal READ leftTurnSignal NOTIFY telemetryChanged)
    Q_PROPERTY(bool rightTurnSignal READ rightTurnSignal NOTIFY telemetryChanged)
    Q_PROPERTY(int followingDistanceLevel READ followingDistanceLevel NOTIFY telemetryChanged)
    Q_PROPERTY(bool laneWarning READ laneWarning NOTIFY telemetryChanged)
    Q_PROPERTY(bool lowFuelWarning READ lowFuelWarning NOTIFY telemetryChanged)
    Q_PROPERTY(int drivingMode READ drivingMode WRITE setDrivingMode NOTIFY telemetryChanged)
    Q_PROPERTY(QString drivingModeLabel READ drivingModeLabel NOTIFY telemetryChanged)
    Q_PROPERTY(QColor drivingModeAccentColor READ drivingModeAccentColor NOTIFY telemetryChanged)
    Q_PROPERTY(double efficiencyPercent READ efficiencyPercent NOTIFY telemetryChanged)
    Q_PROPERTY(QVariantList tirePressures READ tirePressures NOTIFY telemetryChanged)
    Q_PROPERTY(bool tirePressureWarning READ tirePressureWarning NOTIFY telemetryChanged)
    Q_PROPERTY(int lowTireWheelIndex READ lowTireWheelIndex NOTIFY telemetryChanged)
public:
    explicit VehicleTelemetry(QObject* parent = nullptr);

    [[nodiscard]] double speedKph() const { return m_state.speedKph; }
    [[nodiscard]] double rpm() const { return m_state.rpm; }
    [[nodiscard]] QString gearLabel() const;
    [[nodiscard]] double fuelLevel() const { return m_state.fuelLevelPercent; }
    [[nodiscard]] bool leftTurnSignal() const { return m_state.leftTurnSignal; }
    [[nodiscard]] bool rightTurnSignal() const { return m_state.rightTurnSignal; }
    [[nodiscard]] int followingDistanceLevel() const { return m_state.followingDistanceLevel; }
    [[nodiscard]] bool laneWarning() const { return m_state.laneWarning; }
    [[nodiscard]] bool lowFuelWarning() const { return m_state.lowFuelWarning; }
    [[nodiscard]] int drivingMode() const { return static_cast<int>(m_state.drivingMode); }
    void setDrivingMode(int mode);
    [[nodiscard]] QString drivingModeLabel() const;
    [[nodiscard]] QColor drivingModeAccentColor() const;
    [[nodiscard]] double efficiencyPercent() const { return m_state.efficiencyPercent; }
    [[nodiscard]] QVariantList tirePressures() const;
    [[nodiscard]] bool tirePressureWarning() const { return m_state.tirePressureWarning; }
    [[nodiscard]] int lowTireWheelIndex() const { return m_state.lowTireWheelIndex; }

signals:
    void telemetryChanged();

private:
    void tick();

    DriveCycleSimulator m_simulator;
    VehicleState m_state;
    QTimer m_timer;
};

} // namespace qttutorial::automotive
