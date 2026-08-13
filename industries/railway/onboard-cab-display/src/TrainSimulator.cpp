// SPDX-License-Identifier: MIT
#include "TrainSimulator.h"
#include "BrakingCurve.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace qttutorial::cab_display {

namespace {
constexpr double kKmhToMs = 1000.0 / 3600.0;
constexpr double kMsToKmh = 3600.0 / 1000.0;
constexpr double kYellowLookaheadM = 1200.0;
constexpr int kTickIntervalMs = 100;
constexpr double kTickIntervalSeconds = kTickIntervalMs / 1000.0;
}

TrainSimulator::TrainSimulator(RouteProfile route, QObject* parent)
    : QObject(parent)
    , m_route(std::move(route))
{
    m_timer.setInterval(kTickIntervalMs);
    connect(&m_timer, &QTimer::timeout, this, [this] { tick(kTickIntervalSeconds); });
    m_timer.start();
}

void TrainSimulator::tick(double dtSeconds)
{
    if (m_dwellRemainingSeconds > 0.0) {
        m_dwellRemainingSeconds -= dtSeconds;
        m_speedKmh = 0.0;
        m_doorsOpen = true;
        m_signalAspect = SignalAspect::Red;
        if (m_dwellRemainingSeconds <= 0.0) {
            m_dwellRemainingSeconds = 0.0;
            m_doorsOpen = false;
        }
        emit updated();
        return;
    }

    const double currentSegmentSpeed = m_route.permittedSpeedAt(m_positionM);
    const auto restriction = m_route.nextRestriction(m_positionM);
    const auto station = m_route.nextStation(m_positionM);

    double safeForRestriction = std::numeric_limits<double>::infinity();
    double restrictionDistance = std::numeric_limits<double>::infinity();
    double restrictionSpeed = currentSegmentSpeed;
    if (restriction) {
        safeForRestriction
            = maxSafeSpeedKmh(restriction->distanceToStartM, restriction->permittedSpeedKmh, kServiceDecelerationMs2);
        restrictionDistance = restriction->distanceToStartM;
        restrictionSpeed = restriction->permittedSpeedKmh;
    }

    double safeForStation = std::numeric_limits<double>::infinity();
    double stationDistance = std::numeric_limits<double>::infinity();
    if (station) {
        stationDistance = station->distanceM - m_positionM;
        safeForStation = maxSafeSpeedKmh(stationDistance, 0.0, kServiceDecelerationMs2);
    }

    m_permittedSpeedKmh = std::min({currentSegmentSpeed, safeForRestriction, safeForStation});

    if (safeForStation <= safeForRestriction && station) {
        m_distanceToRestrictionM = stationDistance;
        m_restrictionSpeedKmh = 0.0;
    } else if (restriction) {
        m_distanceToRestrictionM = restrictionDistance;
        m_restrictionSpeedKmh = restrictionSpeed;
    } else {
        m_distanceToRestrictionM = std::numeric_limits<double>::infinity();
        m_restrictionSpeedKmh = currentSegmentSpeed;
    }

    const double maxRateKmhPerS = (m_speedKmh < m_permittedSpeedKmh ? kMaxAccelerationMs2 : kServiceDecelerationMs2)
        * kMsToKmh;
    const double delta = m_permittedSpeedKmh - m_speedKmh;
    const double step = std::clamp(delta, -maxRateKmhPerS * dtSeconds, maxRateKmhPerS * dtSeconds);
    m_speedKmh = std::max(0.0, m_speedKmh + step);

    const double previousPositionM = m_positionM;
    m_positionM += m_speedKmh * kKmhToMs * dtSeconds;

    if (station) {
        m_nextStationName = station->name;
        m_nextStationDistanceM = stationDistance;
    }

    // The braking curve only asymptotically reaches zero at the station
    // itself, so with discrete time steps the train would otherwise glide
    // through at a few km/h without ever registering as "stopped". Treat
    // crossing (or landing within tolerance of) the station this tick as
    // arrival, and snap to a clean stop there.
    const bool arrivedThisTick = station
        && (previousPositionM < station->distanceM) && (m_positionM >= station->distanceM - kStationStopToleranceM);
    if (arrivedThisTick) {
        m_positionM = station->distanceM;
    }

    if (m_route.totalLength() > 0.0 && m_positionM >= m_route.totalLength()) {
        m_positionM -= m_route.totalLength();
    }

    if (arrivedThisTick) {
        m_speedKmh = 0.0;
        m_dwellRemainingSeconds = kDwellSeconds;
        m_doorsOpen = true;
        m_signalAspect = SignalAspect::Red;
    } else {
        m_doorsOpen = false;
        m_signalAspect
            = (m_distanceToRestrictionM <= kYellowLookaheadM) ? SignalAspect::Yellow : SignalAspect::Green;
    }

    emit updated();
}

} // namespace qttutorial::cab_display
