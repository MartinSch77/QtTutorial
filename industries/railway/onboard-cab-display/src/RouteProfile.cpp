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
