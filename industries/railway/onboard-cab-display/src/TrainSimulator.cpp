// SPDX-License-Identifier: MIT
#include "TrainSimulator.h"
#include "BrakingCurve.h"

#include <QVariantMap>

#include <algorithm>
#include <cmath>
#include <limits>

namespace qttutorial::cab_display {

namespace {
constexpr double kKmhToMs = 1000.0 / 3600.0;
constexpr double kMsToKmh = 3600.0 / 1000.0;
constexpr double kYellowLookaheadM = 1200.0;
constexpr double kGravityMs2 = 9.81;
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
        m_brakeWarningActive = false;
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

    // The braking-curve function above assumes a flat track. Actual traction
    // and braking effort here is adjusted for the segment's real gradient
    // (gravity helps braking uphill, hinders it downhill), so a steep enough
    // downhill segment can make the train's real deceleration fall short of
    // what the curve assumed when it set `m_permittedSpeedKmh` — a genuine,
    // physically-motivated overspeed rather than a discretisation artefact.
    const double gravityTermMs2 = kGravityMs2 * (m_route.gradientPercentAt(m_positionM) / 100.0);
    const double effectiveAccelerationMs2 = std::max(0.05, kMaxAccelerationMs2 - gravityTermMs2);
    const double effectiveDecelerationMs2 = std::max(0.05, kServiceDecelerationMs2 + gravityTermMs2);

    const double maxRateKmhPerS
        = (m_speedKmh < m_permittedSpeedKmh ? effectiveAccelerationMs2 : effectiveDecelerationMs2) * kMsToKmh;
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

    m_brakeWarningActive = nextBrakeWarningState(m_brakeWarningActive, m_speedKmh, m_permittedSpeedKmh,
                                                  kBrakeWarningOnMarginKmh, kBrakeWarningOffMarginKmh);

    emit updated();
}

QVariantList TrainSimulator::upcomingRestrictions() const
{
    const std::vector<Restriction> restrictions
        = m_route.upcomingRestrictions(m_positionM, kPlanningLookaheadM, kPlanningMaxRestrictions);
    QVariantList list;
    list.reserve(static_cast<qsizetype>(restrictions.size()));
    for (const Restriction& restriction : restrictions) {
        QVariantMap entry;
        entry.insert(QStringLiteral("distanceM"), restriction.distanceToStartM);
        entry.insert(QStringLiteral("speedKmh"), restriction.permittedSpeedKmh);
        list.append(entry);
    }
    return list;
}

} // namespace qttutorial::cab_display
