// SPDX-License-Identifier: MIT
#include "DataLinkTrackModel.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace qttutorial::defence {

namespace {
double normalizeDeg(double deg)
{
    double result = std::fmod(deg, 360.0);
    if (result < 0.0) {
        result += 360.0;
    }
    return result;
}
}

DataLinkTrackModel::DataLinkTrackModel(double areaRadiusKm)
    : m_areaRadiusKm(areaRadiusKm)
{
    seedTracks();
}

void DataLinkTrackModel::seedTracks()
{
    const struct { int id; const char* classification; const char* domain; double xKm; double yKm;
                   double headingDeg; double speedKmh; double turnRateDegPerSec; } seeds[] = {
        {101, "Rotary-wing", "Air", 22.0, -14.0, 300.0, 140.0, -1.8},
        {102, "Surface vessel", "Surface", -18.0, 22.0, 60.0, 24.0, 0.6},
    };

    m_tracks.clear();
    m_trueXKm.clear();
    m_trueYKm.clear();
    m_trueHeadingDeg.clear();
    m_turnRateDegPerSec.clear();
    m_ticksSinceUpdate.clear();

    for (const auto& seed : seeds) {
        DataLinkTrack track;
        track.id = seed.id;
        track.classification = QString::fromLatin1(seed.classification);
        track.domain = QString::fromLatin1(seed.domain);
        track.xKm = seed.xKm;
        track.yKm = seed.yKm;
        track.headingDeg = seed.headingDeg;
        track.speedKmh = seed.speedKmh;
        m_tracks.push_back(track);

        m_trueXKm.push_back(seed.xKm);
        m_trueYKm.push_back(seed.yKm);
        m_trueHeadingDeg.push_back(seed.headingDeg);
        m_turnRateDegPerSec.push_back(seed.turnRateDegPerSec);
        m_ticksSinceUpdate.push_back(0);
    }
}

int DataLinkTrackModel::updateIntervalTicks(double linkQualityPercent)
{
    const double clamped = std::clamp(linkQualityPercent, 0.0, 100.0);
    return 1 + static_cast<int>((100.0 - clamped) / 15.0);
}

void DataLinkTrackModel::advance(double dtSeconds, double linkQualityPercent)
{
    const int interval = updateIntervalTicks(linkQualityPercent);

    for (std::size_t i = 0; i < m_tracks.size(); ++i) {
        // The "true" position always advances, exactly like an organic
        // track, representing the other platform continuing to move even
        // while our data link to it is degraded.
        m_trueHeadingDeg[i] = normalizeDeg(m_trueHeadingDeg[i] + m_turnRateDegPerSec[i] * dtSeconds);
        const double headingRad = m_trueHeadingDeg[i] * std::numbers::pi / 180.0;
        const double speedKmPerSec = m_tracks[i].speedKmh / 3600.0;
        double newX = m_trueXKm[i] + std::cos(headingRad) * speedKmPerSec * dtSeconds;
        double newY = m_trueYKm[i] + std::sin(headingRad) * speedKmPerSec * dtSeconds;

        const double radius = std::hypot(newX, newY);
        if (radius > m_areaRadiusKm) {
            const double scale = m_areaRadiusKm / radius;
            newX *= scale;
            newY *= scale;
            m_trueHeadingDeg[i] = normalizeDeg(std::atan2(-newY, -newX) * 180.0 / std::numbers::pi);
        }
        m_trueXKm[i] = newX;
        m_trueYKm[i] = newY;

        // The *displayed* track only refreshes when the simulated data link
        // "delivers" - i.e. once every `interval` ticks. Between deliveries
        // it stays frozen at the last-known-good position and ages, which
        // is exactly what a real off-board track display shows during a
        // comms outage: a stale plot, not a moving one.
        DataLinkTrack& track = m_tracks[i];
        ++m_ticksSinceUpdate[i];
        if (m_ticksSinceUpdate[i] >= interval) {
            track.xKm = m_trueXKm[i];
            track.yKm = m_trueYKm[i];
            track.headingDeg = m_trueHeadingDeg[i];
            track.stale = false;
            track.dataAgeSeconds = 0.0;
            m_ticksSinceUpdate[i] = 0;
        } else {
            track.stale = true;
            track.dataAgeSeconds += dtSeconds;
        }
    }
}

} // namespace qttutorial::defence
