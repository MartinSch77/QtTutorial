// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <QVector3D>
#include <array>
#include <random>
#include <vector>

namespace qttutorial::offboard_digital_twin {

// Operating state of a single simulated machine, driving both the 3D scene
// material color and the equipment tree/right-hand info panel.
enum class MachineState { Normal, Warning, Alarm, Stopped, Maintenance };

// One machine in the digital twin: static placement in the 3D scene plus the
// live telemetry values the rest of the app renders/animates. Deliberately a
// plain struct (no QObject) - MachineTelemetrySimulator below is the single
// QObject that owns a collection of these and emits change notifications, so
// the struct itself stays trivially copyable and easy to unit test.
struct MachineTelemetry {
    int id = -1;
    QString name;
    QString kind; // "conveyor", "press", "cnc-mill", "fan-array" - selects the Machine3D visual
    QVector3D position;
    double temperatureC = 25.0;
    double vibrationMm = 0.2;
    double speedRpm = 0.0;
    MachineState state = MachineState::Normal;
    QString defectivePart; // non-empty once diagnose() identifies a fault
};

// In-process telemetry source for every machine on the factory floor.
//
// A real deployment would replace this with data arriving over MQTT (Qt6::Mqtt
// - see MqttTelemetryBridge, guarded to build only when that module is
// available) or OPC UA/gRPC; here the simulator *is* the data source, so the
// whole app is runnable standalone with no broker. tick() is the pure,
// deterministic-apart-from-noise step function the QTest suite drives
// directly without an event loop; the constructor additionally arms a QTimer
// that calls tick() in real time for the running application.
class MachineTelemetrySimulator : public QObject {
    Q_OBJECT

public:
    explicit MachineTelemetrySimulator(QObject *parent = nullptr);

    [[nodiscard]] int machineCount() const { return static_cast<int>(m_machines.size()); }
    [[nodiscard]] const MachineTelemetry &machineAt(int index) const { return m_machines.at(static_cast<std::size_t>(index)); }
    [[nodiscard]] const std::vector<MachineTelemetry> &machines() const { return m_machines; }
    [[nodiscard]] int indexOfId(int id) const;

    // Pure simulation step (seconds since last tick). Exercised directly by
    // tests; also called every 200ms by the internal QTimer at run time.
    void tick(double dtSeconds);

    // Demo-scenario hooks (step 2/5 of the acceptance-test walkthrough).
    void beginOverheat(int machineId);
    void clearFault(int machineId);
    void setStopped(int machineId, bool stopped);

public slots:
    void start();
    void stop();

signals:
    void machineTelemetryChanged(int machineId);
    void machineStateChanged(int machineId, MachineState state);

private:
    std::vector<MachineTelemetry> m_machines;
    QTimer m_timer;
    std::mt19937 m_rng{12345u}; // fixed seed: deterministic-enough noise for tests/demo repeatability
    std::uniform_real_distribution<double> m_noise{-0.05, 0.05};
    int m_overheatingMachineId = -1;
    double m_overheatElapsed = 0.0;
};

} // namespace qttutorial::offboard_digital_twin
