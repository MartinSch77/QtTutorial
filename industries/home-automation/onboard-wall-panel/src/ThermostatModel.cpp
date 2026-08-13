// SPDX-License-Identifier: MIT
#include "ThermostatModel.h"

#include <algorithm>

namespace qttutorial::homeautomation {

namespace {
double moveToward(double current, double target, double maxDelta)
{
    if (maxDelta <= 0.0 || current == target) {
        return current;
    }
    if (current < target) {
        return std::min(current + maxDelta, target);
    }
    return std::max(current - maxDelta, target);
}
} // namespace

ThermostatModel::ThermostatModel() = default;

void ThermostatModel::advance(double deltaSeconds)
{
    double driveTarget = m_currentTemperature;
    double rate = 0.0;

    switch (m_mode) {
    case Mode::Off:
        driveTarget = m_outsideAmbient;
        rate = kDriftRatePerSecond;
        break;
    case Mode::Heat:
        if (m_currentTemperature < m_targetTemperature) {
            driveTarget = m_targetTemperature;
            rate = kHeatCoolRatePerSecond;
        }
        break;
    case Mode::Cool:
        if (m_currentTemperature > m_targetTemperature) {
            driveTarget = m_targetTemperature;
            rate = kHeatCoolRatePerSecond;
        }
        break;
    }

    m_currentTemperature = moveToward(m_currentTemperature, driveTarget, rate * deltaSeconds);
}

} // namespace qttutorial::homeautomation
