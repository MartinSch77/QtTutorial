// SPDX-License-Identifier: MIT
#include "MachineSimulator.h"

#include <QRandomGenerator>

#include <algorithm>
#include <cmath>

namespace qttutorial::factory_machine_panel {

namespace {
constexpr double kSpeedTimeConstant = 2.0;
constexpr double kTemperatureTimeConstant = 3.0;
constexpr double kVibrationTimeConstant = 1.0;
constexpr double kTemperatureNoiseAmplitude = 1.0;
constexpr double kVibrationNoiseAmplitude = 0.3;
constexpr int kTickIntervalMs = 100;
constexpr double kTickIntervalSeconds = kTickIntervalMs / 1000.0;
}

MachineSimulator::MachineSimulator(QObject* parent)
    : QObject(parent)
{
    m_timer.setInterval(kTickIntervalMs);
    connect(&m_timer, &QTimer::timeout, this, [this] { tick(kTickIntervalSeconds); });
    m_timer.start();
}

double MachineSimulator::noise(double amplitude)
{
    return (QRandomGenerator::global()->generateDouble() * 2.0 - 1.0) * amplitude;
}

void MachineSimulator::setRunning(bool running)
{
    if (m_running == running) {
        return;
    }
    m_running = running;
    emit runningChanged(m_running);
}

bool MachineSimulator::setTargetSpeed(double rpm)
{
    if (!std::isfinite(rpm) || rpm < kMinSetpoint || rpm > kMaxSetpoint) {
        emit setpointRejected(QStringLiteral("Setpoint must be between %1 and %2 RPM")
                                   .arg(kMinSetpoint)
                                   .arg(kMaxSetpoint));
        return false;
    }
    if (qFuzzyCompare(m_targetSpeed + 1.0, rpm + 1.0)) {
        return true;
    }
    m_targetSpeed = rpm;
    emit targetSpeedChanged(m_targetSpeed);
    return true;
}

void MachineSimulator::tick(double dtSeconds)
{
    const double effectiveTarget = m_running ? m_targetSpeed : 0.0;
    m_speed += (effectiveTarget - m_speed) * (dtSeconds / kSpeedTimeConstant);
    m_speed = std::clamp(m_speed, 0.0, kMaxSetpoint);
    emit speedChanged(m_speed);

    const double targetTemperature = kAmbientTemperature + m_speed * kTempPerRpmUnit;
    m_temperature += (targetTemperature - m_temperature) * (dtSeconds / kTemperatureTimeConstant);
    m_temperature += noise(kTemperatureNoiseAmplitude) * dtSeconds;
    emit temperatureChanged(m_temperature);

    const double targetVibration = m_speed * kVibrationPerRpmUnit;
    m_vibration += (targetVibration - m_vibration) * (dtSeconds / kVibrationTimeConstant);
    m_vibration = std::max(0.0, m_vibration + noise(kVibrationNoiseAmplitude) * dtSeconds);
    emit vibrationChanged(m_vibration);

    if (m_temperature >= kTempFaultThreshold) {
        emit temperatureFault();
    } else if (m_temperature >= kTempWarningThreshold) {
        emit temperatureWarning();
    }

    if (m_vibration >= kVibrationFaultThreshold) {
        emit vibrationFault();
    } else if (m_vibration >= kVibrationWarningThreshold) {
        emit vibrationWarning();
    }

    if (m_running) {
        m_cycleProgress += dtSeconds;
        if (m_cycleProgress >= kCycleDurationSeconds) {
            m_cycleProgress -= kCycleDurationSeconds;
            ++m_cycleCount;
            emit cycleCountChanged(m_cycleCount);
            emit cycleCompleted();
        }
        emit cycleProgressChanged(cycleProgress());
    } else if (m_cycleProgress != 0.0) {
        m_cycleProgress = 0.0;
        emit cycleProgressChanged(cycleProgress());
    }
}

} // namespace qttutorial::factory_machine_panel
