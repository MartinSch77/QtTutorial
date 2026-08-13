// SPDX-License-Identifier: MIT
#include "FieldPassSimulator.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace qttutorial::agriculture {

namespace {
constexpr double kTwoPi = 2.0 * std::numbers::pi;
}

bool FieldPassSimulator::isImplementEngagedAt(double progressFraction)
{
    return progressFraction >= FieldPassSimulator::kTurnZoneFraction
        && progressFraction <= (1.0 - FieldPassSimulator::kTurnZoneFraction);
}

double FieldPassSimulator::workingDepthAt(bool engaged)
{
    return engaged ? kWorkingDepthCm : 0.0;
}

double FieldPassSimulator::crossTrackErrorAt(double totalDistanceMeters)
{
    const double slowWander = 6.0 * std::sin(kTwoPi * totalDistanceMeters / 50.0);
    const double fastCorrection = 3.0 * std::sin(kTwoPi * totalDistanceMeters / 17.0);
    return slowWander + fastCorrection;
}

double FieldPassSimulator::engineLoadAt(bool engaged, double crossTrackErrorCm)
{
    if (!engaged) {
        return 18.0 + std::min(std::abs(crossTrackErrorCm), 5.0) * 0.4;
    }
    return std::clamp(70.0 + std::abs(crossTrackErrorCm) * 1.5, 60.0, 95.0);
}

double FieldPassSimulator::speedKphAt(bool engaged)
{
    return engaged ? kWorkingSpeedKph : kTurnSpeedKph;
}

double FieldPassSimulator::fuelPercentPerKmAt(double engineLoadPercent)
{
    return kFuelBaselinePercentPerKm + engineLoadPercent * kFuelPercentPerKmPerLoadPoint;
}

double FieldPassSimulator::yieldRateAt(bool engaged, double speedKph)
{
    if (!engaged) {
        return 0.0;
    }
    return kYieldTonsPerHourAtWorkingSpeed * (speedKph / kWorkingSpeedKph);
}

int FieldPassSimulator::rowIndexForPass(int passNumber)
{
    const int zeroBased = std::max(0, passNumber - 1);
    return zeroBased % kFieldRowCount;
}

bool FieldPassSimulator::isMovingForward(int rowIndex)
{
    return rowIndex % 2 == 0;
}

FieldPassSimulator::FieldPassSimulator(ImplementKind implementKind)
{
    m_state.implementKind = implementKind;
    m_state.rowIndex = rowIndexForPass(m_state.passNumber);
    m_state.movingForward = isMovingForward(m_state.rowIndex);
}

FieldPassState FieldPassSimulator::advance(double dtSeconds)
{
    const double progressFraction = m_distanceInPassMeters / kPassLengthMeters;
    const bool engagedNow = isImplementEngagedAt(progressFraction);
    const double speedKph = speedKphAt(engagedNow);
    const double distanceDeltaMeters = speedKph * 1000.0 / 3600.0 * dtSeconds;

    m_distanceInPassMeters += distanceDeltaMeters;
    m_totalDistanceMeters += distanceDeltaMeters;

    if (m_distanceInPassMeters >= kPassLengthMeters) {
        m_distanceInPassMeters = std::fmod(m_distanceInPassMeters, kPassLengthMeters);
        m_state.passNumber += 1;
    }

    const double newProgressFraction = m_distanceInPassMeters / kPassLengthMeters;
    const bool engaged = isImplementEngagedAt(newProgressFraction);

    m_state.coveragePercent = newProgressFraction * 100.0;
    m_state.crossTrackErrorCm = crossTrackErrorAt(m_totalDistanceMeters);
    m_state.implementEngaged = engaged;
    m_state.workingDepthCm = workingDepthAt(engaged);
    m_state.engineLoadPercent = engineLoadAt(engaged, m_state.crossTrackErrorCm);
    m_state.yieldRateTonsPerHour = yieldRateAt(engaged, speedKphAt(engaged));
    m_state.rowIndex = rowIndexForPass(m_state.passNumber);
    m_state.movingForward = isMovingForward(m_state.rowIndex);

    const double fuelPercentPerKm = fuelPercentPerKmAt(m_state.engineLoadPercent);
    m_fuelLevelPercent -= (distanceDeltaMeters / 1000.0) * fuelPercentPerKm;
    if (m_fuelLevelPercent <= 0.0) {
        m_fuelLevelPercent = 100.0;
    }
    m_state.fuelLevelPercent = m_fuelLevelPercent;

    return m_state;
}

} // namespace qttutorial::agriculture
