// SPDX-License-Identifier: MIT
#pragma once

#include <QObject>
#include <QTimer>

namespace qttutorial::factory_machine_panel {

// Simulates the sensor feed of a single physical machine (e.g. a CNC mill or
// packaging press): speed (RPM) ramps toward an operator-set target with a
// first-order lag, temperature and vibration follow speed with their own lag
// plus small random noise/drift, and a production cycle counter advances
// while the machine is running. tick() is the pure, deterministic-enough
// step function (noise aside) that the QTest suite drives directly, without
// an event loop or real time passing; the constructor additionally arms a
// QTimer that drives it in real time for the running application.
class MachineSimulator : public QObject {
    Q_OBJECT
    Q_PROPERTY(double speed READ speed NOTIFY speedChanged)
    Q_PROPERTY(double temperature READ temperature NOTIFY temperatureChanged)
    Q_PROPERTY(double vibration READ vibration NOTIFY vibrationChanged)
    Q_PROPERTY(int cycleCount READ cycleCount NOTIFY cycleCountChanged)
    Q_PROPERTY(double cycleProgress READ cycleProgress NOTIFY cycleProgressChanged)
    Q_PROPERTY(double targetSpeed READ targetSpeed NOTIFY targetSpeedChanged)
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)

public:
    static constexpr double kMinSetpoint = 0.0;
    static constexpr double kMaxSetpoint = 3000.0; // RPM
    static constexpr double kAmbientTemperature = 25.0; // degrees C, machine at rest
    static constexpr double kTempPerRpmUnit = 0.025; // degrees C per RPM at steady state
    static constexpr double kTempWarningThreshold = 80.0;
    static constexpr double kTempFaultThreshold = 95.0;
    static constexpr double kVibrationPerRpmUnit = 0.0025; // mm/s per RPM at steady state
    static constexpr double kVibrationWarningThreshold = 5.0;
    static constexpr double kVibrationFaultThreshold = 7.0;
    static constexpr double kCycleDurationSeconds = 4.0;

    explicit MachineSimulator(QObject* parent = nullptr);

    [[nodiscard]] double speed() const { return m_speed; }
    [[nodiscard]] double temperature() const { return m_temperature; }
    [[nodiscard]] double vibration() const { return m_vibration; }
    [[nodiscard]] int cycleCount() const { return m_cycleCount; }
    [[nodiscard]] double cycleProgress() const { return m_cycleProgress / kCycleDurationSeconds; }
    [[nodiscard]] double targetSpeed() const { return m_targetSpeed; }
    [[nodiscard]] bool isRunning() const { return m_running; }

public slots:
    void setRunning(bool running);
    // Validates and applies a new operator setpoint. Rejects (leaving the
    // existing target untouched and emitting setpointRejected()) values
    // outside [kMinSetpoint, kMaxSetpoint] or values that fail to parse.
    bool setTargetSpeed(double rpm);
    // Advances the simulation by dtSeconds. Deterministic apart from the
    // small bounded noise term, so tests can drive it directly.
    void tick(double dtSeconds);

signals:
    void speedChanged(double value);
    void temperatureChanged(double value);
    void vibrationChanged(double value);
    void cycleCountChanged(int value);
    void cycleProgressChanged(double value);
    void cycleCompleted();
    void targetSpeedChanged(double value);
    void runningChanged(bool running);
    void setpointRejected(QString reason);
    void temperatureWarning();
    void temperatureFault();
    void vibrationWarning();
    void vibrationFault();

private:
    [[nodiscard]] static double noise(double amplitude);

    double m_speed = 0.0;
    double m_temperature = kAmbientTemperature;
    double m_vibration = 0.0;
    double m_targetSpeed = 0.0;
    double m_cycleProgress = 0.0;
    int m_cycleCount = 0;
    bool m_running = false;
    QTimer m_timer;
};

} // namespace qttutorial::factory_machine_panel
