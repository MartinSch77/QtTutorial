// SPDX-License-Identifier: GPL-3.0-or-later
#include "MachineTelemetrySimulator.h"

#include <algorithm>
#include <cmath>

namespace qttutorial::offboard_digital_twin {

namespace {
constexpr double kAmbientTemperature = 25.0;
constexpr double kAlarmTemperature = 92.0;
constexpr double kWarningTemperature = 70.0;
constexpr double kOverheatRampSeconds = 6.0; // matches the "few seconds" color ramp the spec asks for
}

MachineTelemetrySimulator::MachineTelemetrySimulator(QObject *parent)
    : QObject(parent)
    , m_machines{
          MachineTelemetry{0, QStringLiteral("Conveyor A1"), QStringLiteral("conveyor"),
                            QVector3D(-3.0f, 0.0f, -1.5f), 32.0, 0.3, 180.0, MachineState::Normal, {}},
          MachineTelemetry{1, QStringLiteral("CNC Mill B2"), QStringLiteral("cnc-mill"),
                            QVector3D(0.0f, 0.0f, -1.5f), 41.0, 0.4, 950.0, MachineState::Normal, {}},
          MachineTelemetry{2, QStringLiteral("Cooling Fan Array C1"), QStringLiteral("fan-array"),
                            QVector3D(3.0f, 0.0f, -1.5f), 29.0, 0.2, 1200.0, MachineState::Normal, {}},
          MachineTelemetry{3, QStringLiteral("Hydraulic Press D3"), QStringLiteral("press"),
                            QVector3D(-1.5f, 0.0f, 1.8f), 38.0, 0.5, 60.0, MachineState::Normal, {}},
      }
{
    m_timer.setInterval(200);
    connect(&m_timer, &QTimer::timeout, this, [this]() { tick(0.2); });
}

int MachineTelemetrySimulator::indexOfId(int id) const
{
    for (std::size_t i = 0; i < m_machines.size(); ++i) {
        if (m_machines[i].id == id)
            return static_cast<int>(i);
    }
    return -1;
}

void MachineTelemetrySimulator::start() { m_timer.start(); }
void MachineTelemetrySimulator::stop() { m_timer.stop(); }

void MachineTelemetrySimulator::tick(double dtSeconds)
{
    for (auto &machine : m_machines) {
        if (machine.state == MachineState::Stopped)
            continue;

        const double noise = m_noise(m_rng);

        if (machine.id == m_overheatingMachineId) {
            m_overheatElapsed += dtSeconds;
            const double rampFraction = std::clamp(m_overheatElapsed / kOverheatRampSeconds, 0.0, 1.0);
            machine.temperatureC = kAmbientTemperature + rampFraction * (kAlarmTemperature + 10.0 - kAmbientTemperature);
            machine.vibrationMm = 0.4 + rampFraction * 2.2 + std::abs(noise);
        } else {
            // Gentle mean-reverting drift around the machine's nominal operating point.
            const double nominal = kAmbientTemperature + machine.speedRpm * 0.012;
            machine.temperatureC += (nominal - machine.temperatureC) * 0.05 + noise;
            machine.vibrationMm = std::max(0.05, machine.vibrationMm + noise * 0.1);
        }

        const MachineState previous = machine.state;
        if (machine.state != MachineState::Maintenance) {
            if (machine.temperatureC >= kAlarmTemperature)
                machine.state = MachineState::Alarm;
            else if (machine.temperatureC >= kWarningTemperature)
                machine.state = MachineState::Warning;
            else
                machine.state = MachineState::Normal;
        }

        emit machineTelemetryChanged(machine.id);
        if (machine.state != previous)
            emit machineStateChanged(machine.id, machine.state);
    }
}

void MachineTelemetrySimulator::beginOverheat(int machineId)
{
    m_overheatingMachineId = machineId;
    m_overheatElapsed = 0.0;
}

void MachineTelemetrySimulator::clearFault(int machineId)
{
    if (m_overheatingMachineId == machineId)
        m_overheatingMachineId = -1;

    const int index = indexOfId(machineId);
    if (index < 0)
        return;
    auto &machine = m_machines[static_cast<std::size_t>(index)];
    machine.temperatureC = kAmbientTemperature + machine.speedRpm * 0.012;
    machine.vibrationMm = 0.2;
    machine.state = MachineState::Normal;
    machine.defectivePart.clear();
    emit machineTelemetryChanged(machine.id);
    emit machineStateChanged(machine.id, machine.state);
}

void MachineTelemetrySimulator::setStopped(int machineId, bool stopped)
{
    const int index = indexOfId(machineId);
    if (index < 0)
        return;
    auto &machine = m_machines[static_cast<std::size_t>(index)];
    machine.state = stopped ? MachineState::Stopped : MachineState::Maintenance;
    if (!stopped)
        machine.defectivePart = QStringLiteral("Bearing housing #4 (worn race)");
    emit machineTelemetryChanged(machine.id);
    emit machineStateChanged(machine.id, machine.state);
}

} // namespace qttutorial::offboard_digital_twin
