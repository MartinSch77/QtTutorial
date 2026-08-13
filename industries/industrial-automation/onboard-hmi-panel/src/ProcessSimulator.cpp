// SPDX-License-Identifier: MIT
#include "ProcessSimulator.h"

#include <algorithm>
#include <cmath>

namespace qttutorial::industrial_hmi {

namespace {
constexpr double kFlowTimeConstant = 1.5;
constexpr double kPressureTimeConstant = 0.8;
constexpr double kPressurePerFlowUnit = 0.9;
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

void ProcessSimulator::tick(double dtSeconds)
{
    const double targetInflow = m_pumpRunning ? kMaxInflowRate : 0.0;
    m_flowRate += (targetInflow - m_flowRate) * (dtSeconds / kFlowTimeConstant);

    const double netFlow = m_flowRate - kOutflowRate;
    const double previousLevel = m_tankLevel;
    m_tankLevel = std::clamp(m_tankLevel + netFlow * dtSeconds, 0.0, kTankCapacityPercent);

    const double targetPressure = m_flowRate * kPressurePerFlowUnit;
    m_pressure += (targetPressure - m_pressure) * (dtSeconds / kPressureTimeConstant);

    if (!qFuzzyCompare(m_tankLevel + 1.0, previousLevel + 1.0)) {
        emit tankLevelChanged(m_tankLevel);
    }
    emit flowRateChanged(m_flowRate);
    emit pressureChanged(m_pressure);

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
