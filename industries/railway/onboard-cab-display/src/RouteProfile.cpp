// SPDX-License-Identifier: MIT
#include "RouteProfile.h"

#include <algorithm>

namespace qttutorial::cab_display {

RouteProfile::RouteProfile(std::vector<SpeedSegment> segments, std::vector<Station> stations)
    : m_segments(std::move(segments))
    , m_stations(std::move(stations))
{
}

double RouteProfile::permittedSpeedAt(double distanceM) const
{
    for (const SpeedSegment& segment : m_segments) {
        if (distanceM >= segment.startDistanceM && distanceM < segment.endDistanceM) {
            return segment.permittedSpeedKmh;
        }
    }
    return m_segments.empty() ? 0.0 : m_segments.back().permittedSpeedKmh;
}

double RouteProfile::gradientPercentAt(double distanceM) const
{
    for (const SpeedSegment& segment : m_segments) {
        if (distanceM >= segment.startDistanceM && distanceM < segment.endDistanceM) {
            return segment.gradientPercent;
        }
    }
    return m_segments.empty() ? 0.0 : m_segments.back().gradientPercent;
}

std::optional<Restriction> RouteProfile::nextRestriction(double distanceM) const
{
    const double currentSpeed = permittedSpeedAt(distanceM);
    for (const SpeedSegment& segment : m_segments) {
        if (segment.startDistanceM > distanceM && segment.permittedSpeedKmh < currentSpeed) {
            return Restriction{segment.startDistanceM - distanceM, segment.permittedSpeedKmh};
        }
    }
    return std::nullopt;
}

std::vector<Restriction> RouteProfile::upcomingRestrictions(double distanceM, double lookaheadM,
                                                              std::size_t maxCount) const
{
    std::vector<Restriction> restrictions;
    double previousSpeed = permittedSpeedAt(distanceM);
    for (const SpeedSegment& segment : m_segments) {
        if (segment.startDistanceM <= distanceM) {
            continue;
        }
        const double distanceToStartM = segment.startDistanceM - distanceM;
        if (distanceToStartM > lookaheadM) {
            break;
        }
        if (segment.permittedSpeedKmh < previousSpeed) {
            restrictions.push_back(Restriction{distanceToStartM, segment.permittedSpeedKmh});
            if (restrictions.size() >= maxCount) {
                break;
            }
        }
        previousSpeed = segment.permittedSpeedKmh;
    }
    return restrictions;
}

std::optional<Station> RouteProfile::nextStation(double distanceM) const
{
    const auto it = std::ranges::find_if(m_stations, [distanceM](const Station& station) {
        return station.distanceM > distanceM;
    });
    if (it == m_stations.end()) {
        return std::nullopt;
    }
    return *it;
}

double RouteProfile::totalLength() const
{
    return m_segments.empty() ? 0.0 : m_segments.back().endDistanceM;
}

} // namespace qttutorial::cab_display
