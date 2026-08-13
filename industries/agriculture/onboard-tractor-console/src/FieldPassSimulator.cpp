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

    const double fuelPercentPerKm = engaged ? kFuelPercentPerKmWorking : kFuelPercentPerKmIdle;
    m_fuelLevelPercent -= (distanceDeltaMeters / 1000.0) * fuelPercentPerKm;
    if (m_fuelLevelPercent <= 0.0) {
        m_fuelLevelPercent = 100.0;
    }
    m_state.fuelLevelPercent = m_fuelLevelPercent;

    return m_state;
}

} // namespace qttutorial::agriculture
