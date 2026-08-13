// SPDX-License-Identifier: MIT
#include "ProcessSimulator.h"

#include <algorithm>
#include <cmath>

namespace qttutorial::industrial_hmi {

namespace {
constexpr double kFlowTimeConstant = 1.5;
constexpr double kPressureTimeConstant = 0.8;
constexpr double kPressurePerFlowUnit = 0.9;
constexpr double kConveyorTimeConstant = 1.0;
constexpr int kTickIntervalMs = 100;
constexpr double kTickIntervalSeconds = kTickIntervalMs / 1000.0;
}

ProcessSimulator::ProcessSimulator(QObject* parent)
    : QObject(parent)
{
    m_timer.setInterval(kTickIntervalMs);
    connect(&m_timer, &QTimer::timeout, this, [this] { tick(kTickIntervalSeconds); });
    m_timer.start();
}

void ProcessSimulator::setPumpRunning(bool running)
{
    if (m_pumpRunning == running) {
        return;
    }
    m_pumpRunning = running;
    emit pumpRunningChanged(m_pumpRunning);
}

void ProcessSimulator::setValveOpen(bool open)
{
    if (m_valveOpen == open) {
        return;
    }
    m_valveOpen = open;
    emit valveOpenChanged(m_valveOpen);
}

void ProcessSimulator::tick(double dtSeconds)
{
    const double targetInflow = m_pumpRunning ? kMaxInflowRate : 0.0;
    m_flowRate += (targetInflow - m_flowRate) * (dtSeconds / kFlowTimeConstant);

    // The downstream block valve gates the tank's outflow: closed, nothing
    // drains out downstream regardless of how much the pump is pushing in.
    const double effectiveOutflow = m_valveOpen ? kOutflowRate : 0.0;
    const double netFlow = m_flowRate - effectiveOutflow;
    const double previousLevel = m_tankLevel;
    m_tankLevel = std::clamp(m_tankLevel + netFlow * dtSeconds, 0.0, kTankCapacityPercent);

    const double targetPressure = m_flowRate * kPressurePerFlowUnit;
    m_pressure += (targetPressure - m_pressure) * (dtSeconds / kPressureTimeConstant);

    // The conveyor only has product to move, and its motor only has a reason
    // to run, when the valve is open and the pump is actually feeding flow
    // downstream. Otherwise both fall back to zero, just like a real
    // interlocked line would.
    const bool motorShouldRun = m_pumpRunning && m_valveOpen;
    const double targetConveyorSpeed = motorShouldRun ? kMaxConveyorSpeed : 0.0;
    m_conveyorSpeed += (targetConveyorSpeed - m_conveyorSpeed) * (dtSeconds / kConveyorTimeConstant);
    if (std::abs(m_conveyorSpeed) < 1e-6) {
        m_conveyorSpeed = 0.0;
    }

    if (!qFuzzyCompare(m_tankLevel + 1.0, previousLevel + 1.0)) {
        emit tankLevelChanged(m_tankLevel);
    }
    emit flowRateChanged(m_flowRate);
    emit pressureChanged(m_pressure);
    emit conveyorSpeedChanged(m_conveyorSpeed);

    if (m_motorRunning != motorShouldRun) {
        m_motorRunning = motorShouldRun;
        emit motorRunningChanged(m_motorRunning);
    }

    if (m_tankLevel >= kHighLevelAlarm) {
        emit highLevelAlarm();
    }
    if (m_tankLevel <= kLowLevelAlarm) {
        emit lowLevelAlarm();
    }
    if (m_pressure >= kHighPressureAlarm) {
        emit highPressureAlarm();
    }
}

} // namespace qttutorial::industrial_hmi
