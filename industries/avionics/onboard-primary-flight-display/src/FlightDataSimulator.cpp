// SPDX-License-Identifier: MIT
#include "FlightDataSimulator.h"

#include <cmath>

namespace qttutorial::avionics {

namespace {
constexpr int kTickIntervalMs = 33;
constexpr double kTickIntervalS = kTickIntervalMs / 1000.0;
}

FlightDataSimulator::FlightDataSimulator(QObject* parent)
    : QObject(parent)
{
    m_timer.setInterval(kTickIntervalMs);
    connect(&m_timer, &QTimer::timeout, this, &FlightDataSimulator::tick);
}

void FlightDataSimulator::start()
{
    m_timer.start();
}

void FlightDataSimulator::stop()
{
    m_timer.stop();
}

void FlightDataSimulator::tick()
{
    m_elapsedSeconds += kTickIntervalS;

    ControlInput input;
    input.aileron = std::sin(m_elapsedSeconds * 0.15) * 0.6;
    input.elevator = std::sin(m_elapsedSeconds * 0.09 + 1.0) * 0.4;
    input.throttle = 0.55 + std::sin(m_elapsedSeconds * 0.05) * 0.1;

    m_dynamics.setControlInput(input);
    m_dynamics.step(kTickIntervalS);
    emit stateChanged(m_dynamics.state());
}

} // namespace qttutorial::avionics
