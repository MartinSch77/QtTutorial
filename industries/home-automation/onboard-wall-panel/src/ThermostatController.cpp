// SPDX-License-Identifier: MIT
#include "ThermostatController.h"

namespace qttutorial::homeautomation {

namespace {
constexpr int kTickIntervalMs = 200;
constexpr double kTickSeconds = kTickIntervalMs / 1000.0;
}

ThermostatController::ThermostatController(QObject* parent)
    : QObject(parent)
{
    m_model.setMode(ThermostatModel::Mode::Heat);
    m_timer.setInterval(kTickIntervalMs);
    connect(&m_timer, &QTimer::timeout, this, &ThermostatController::tick);
    m_timer.start();
}

void ThermostatController::setTargetTemperature(double celsius)
{
    m_model.setTargetTemperature(celsius);
    emit temperatureChanged();
}

void ThermostatController::setMode(Mode mode)
{
    m_model.setMode(static_cast<ThermostatModel::Mode>(mode));
    emit temperatureChanged();
}

void ThermostatController::tick()
{
    m_model.advance(kTickSeconds);
    emit temperatureChanged();
}

} // namespace qttutorial::homeautomation
