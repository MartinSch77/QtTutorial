// SPDX-License-Identifier: MIT
#include "FlightDynamics.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace qttutorial::avionics {

namespace {

constexpr double kMaxRollDeg = 30.0;
constexpr double kMaxPitchDeg = 15.0;
constexpr double kRollTimeConstantS = 1.5;
constexpr double kPitchTimeConstantS = 2.5;
constexpr double kVerticalSpeedTimeConstantS = 4.0;
constexpr double kAirspeedTimeConstantS = 6.0;
constexpr double kTurnRateDegPerSecPerDegBank = 0.12;
constexpr double kKnotsToFtPerMin = 101.3;
constexpr double kEngineTempTimeConstantS = 12.0;
constexpr double kEngineTempPerThrottleC = 300.0;
constexpr double kEngineTempClimbPenaltyPerDegC = 6.0;

double approach(double current, double target, double dt, double timeConstant)
{
    const double alpha = std::clamp(dt / timeConstant, 0.0, 1.0);
    return current + (target - current) * alpha;
}

} // namespace

void FlightDynamics::setControlInput(const ControlInput& input)
{
    m_input = input;
}

void FlightDynamics::step(double dtSeconds)
{
    if (dtSeconds <= 0.0) {
        return;
    }

    const double targetRoll = std::clamp(m_input.aileron, -1.0, 1.0) * kMaxRollDeg;
    m_state.rollDeg = approach(m_state.rollDeg, targetRoll, dtSeconds, kRollTimeConstantS);

    const double targetPitch = std::clamp(m_input.elevator, -1.0, 1.0) * kMaxPitchDeg;
    m_state.pitchDeg = approach(m_state.pitchDeg, targetPitch, dtSeconds, kPitchTimeConstantS);

    m_state.headingDeg += m_state.rollDeg * kTurnRateDegPerSecPerDegBank * dtSeconds;
    m_state.headingDeg = std::fmod(m_state.headingDeg + 360.0, 360.0);

    const double forwardFtPerMin = m_state.airspeedKt * kKnotsToFtPerMin;
    const double targetVerticalSpeed = forwardFtPerMin * std::sin(m_state.pitchDeg * std::numbers::pi / 180.0);
    m_state.verticalSpeedFtPerMin =
        approach(m_state.verticalSpeedFtPerMin, targetVerticalSpeed, dtSeconds, kVerticalSpeedTimeConstantS);
    m_state.altitudeFt += m_state.verticalSpeedFtPerMin / 60.0 * dtSeconds;
    m_state.altitudeFt = std::max(m_state.altitudeFt, 0.0);

    const double throttle = std::clamp(m_input.throttle, 0.0, 1.0);
    const double climbPenalty = m_state.pitchDeg * 2.0;
    const double targetAirspeed = std::clamp(120.0 + throttle * 220.0 - climbPenalty, 60.0, 400.0);
    m_state.airspeedKt = approach(m_state.airspeedKt, targetAirspeed, dtSeconds, kAirspeedTimeConstantS);

    // Engine temperature couples to the same throttle input, plus a small extra
    // penalty for climbing (positive pitch), so a sustained high-power climb -
    // not random jitter - is what pushes it toward the caution band.
    const double climbTempPenalty = std::max(0.0, m_state.pitchDeg) * kEngineTempClimbPenaltyPerDegC;
    const double targetEngineTemp = kEngineTempNominalC + throttle * kEngineTempPerThrottleC + climbTempPenalty;
    m_state.engineTempC = approach(m_state.engineTempC, targetEngineTemp, dtSeconds, kEngineTempTimeConstantS);
}

} // namespace qttutorial::avionics
