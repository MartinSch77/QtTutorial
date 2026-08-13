// SPDX-License-Identifier: MIT
#pragma once

#include <QObject>
#include <QTimer>

namespace qttutorial::industrial_hmi {

// Simulates a single tank/valve/conveyor/motor process line with believable
// first-order dynamics and plausible cross-coupling between the stages,
// mirroring the tank -> valve -> conveyor -> motor mimic drawn on screen:
//
//   - the tank level integrates the difference between inflow and outflow
//     rather than jumping to a target;
//   - pressure follows flow with a short lag;
//   - the downstream valve gates the tank's outflow: closing it stops
//     product draining out of the tank (the level then rises/holds instead
//     of falling), exactly as closing a real block valve would;
//   - the conveyor motor only has somewhere to move product from when the
//     valve is open *and* the pump is running; otherwise its target speed is
//     zero, and the motor is reported as not running.
//
// tick() is the pure, testable step function; the constructor additionally
// arms a QTimer that drives it in real time for the running application.
class ProcessSimulator : public QObject {
    Q_OBJECT
    Q_PROPERTY(double tankLevel READ tankLevel NOTIFY tankLevelChanged)
    Q_PROPERTY(double flowRate READ flowRate NOTIFY flowRateChanged)
    Q_PROPERTY(double pressure READ pressure NOTIFY pressureChanged)
    Q_PROPERTY(bool pumpRunning READ isPumpRunning NOTIFY pumpRunningChanged)
    Q_PROPERTY(bool valveOpen READ isValveOpen NOTIFY valveOpenChanged)
    Q_PROPERTY(double conveyorSpeed READ conveyorSpeed NOTIFY conveyorSpeedChanged)
    Q_PROPERTY(bool motorRunning READ isMotorRunning NOTIFY motorRunningChanged)

public:
    static constexpr double kTankCapacityPercent = 100.0;
    static constexpr double kMaxInflowRate = 8.0;
    static constexpr double kOutflowRate = 5.0;
    static constexpr double kMaxConveyorSpeed = 1.2;
    static constexpr double kHighLevelAlarm = 90.0;
    static constexpr double kLowLevelAlarm = 10.0;
    static constexpr double kHighPressureAlarm = 7.5;

    explicit ProcessSimulator(QObject* parent = nullptr);

    [[nodiscard]] double tankLevel() const { return m_tankLevel; }
    [[nodiscard]] double flowRate() const { return m_flowRate; }
    [[nodiscard]] double pressure() const { return m_pressure; }
    [[nodiscard]] bool isPumpRunning() const { return m_pumpRunning; }
    [[nodiscard]] bool isValveOpen() const { return m_valveOpen; }
    [[nodiscard]] double conveyorSpeed() const { return m_conveyorSpeed; }
    [[nodiscard]] bool isMotorRunning() const { return m_motorRunning; }

public slots:
    void setPumpRunning(bool running);
    void setValveOpen(bool open);
    // Advances the simulation by dtSeconds. Pure and deterministic, so tests can
    // drive it directly without an event loop or real time passing.
    void tick(double dtSeconds);

signals:
    void tankLevelChanged(double value);
    void flowRateChanged(double value);
    void pressureChanged(double value);
    void pumpRunningChanged(bool running);
    void valveOpenChanged(bool open);
    void conveyorSpeedChanged(double value);
    void motorRunningChanged(bool running);
    void highLevelAlarm();
    void lowLevelAlarm();
    void highPressureAlarm();

private:
    double m_tankLevel = 45.0;
    double m_flowRate = 0.0;
    double m_pressure = 0.0;
    bool m_pumpRunning = false;
    bool m_valveOpen = true;
    double m_conveyorSpeed = 0.0;
    bool m_motorRunning = false;
    QTimer m_timer;
};

} // namespace qttutorial::industrial_hmi
