// SPDX-License-Identifier: MIT
#include "TrackMotionModel.h"

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

TrackSimulator::TrackSimulator(double areaRadiusKm)
    : m_areaRadiusKm(areaRadiusKm)
{
    seedTracks();
}

void TrackSimulator::seedTracks()
{
    const struct { const char* classification; const char* domain; double xKm; double yKm; double headingDeg;
                   double speedKmh; double turnRateDegPerSec; } seeds[] = {
        {"Fixed-wing", "Air", -20.0, 10.0, 90.0, 480.0, 0.0},
        {"Rotary-wing", "Air", 15.0, -8.0, 210.0, 160.0, 1.5},
        {"Surface vessel", "Surface", -5.0, -25.0, 45.0, 32.0, -0.4},
        {"Unknown contact", "Ground", 28.0, 18.0, 260.0, 90.0, 2.2},
        {"Fixed-wing", "Air", 0.0, 30.0, 180.0, 520.0, -1.1},
    };

    m_tracks.clear();
    m_turnRateDegPerSec.clear();
    int id = 1;
    for (const auto& seed : seeds) {
        Track track;
        track.id = id++;
        track.classification = QString::fromLatin1(seed.classification);
        track.domain = QString::fromLatin1(seed.domain);
        track.xKm = seed.xKm;
        track.yKm = seed.yKm;
        track.headingDeg = seed.headingDeg;
        track.speedKmh = seed.speedKmh;
        m_tracks.push_back(track);
        m_turnRateDegPerSec.push_back(seed.turnRateDegPerSec);
    }
}

void TrackSimulator::advance(double dtSeconds)
{
    for (std::size_t i = 0; i < m_tracks.size(); ++i) {
        Track& track = m_tracks[i];
        track.headingDeg = normalizeDeg(track.headingDeg + m_turnRateDegPerSec[i] * dtSeconds);

        const double headingRad = track.headingDeg * std::numbers::pi / 180.0;
        const double speedKmPerSec = track.speedKmh / 3600.0;
        double newX = track.xKm + std::cos(headingRad) * speedKmPerSec * dtSeconds;
        double newY = track.yKm + std::sin(headingRad) * speedKmPerSec * dtSeconds;

        const double radius = std::hypot(newX, newY);
        if (radius > m_areaRadiusKm) {
            const double scale = m_areaRadiusKm / radius;
            newX *= scale;
            newY *= scale;
            track.headingDeg = normalizeDeg(std::atan2(-newY, -newX) * 180.0 / std::numbers::pi);
        }

        track.xKm = newX;
        track.yKm = newY;
    }
}

} // namespace qttutorial::defence
