// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

#include <vector>

namespace qttutorial::defence {

// A single simulated sensor track: position/heading/speed and a passive
// classification label. No weapons data, no aim/intercept solution of any
// kind - this is purely a "what does the operator's scope show" model.
struct Track {
    int id = 0;
    QString classification;
    double xKm = 0.0;
    double yKm = 0.0;
    double headingDeg = 0.0;
    double speedKmh = 0.0;
};

// Advances a handful of simulated tracks around a circular surveillance area
// with smooth, continuous motion (no teleporting). Pure C++23, no Qt event
// loop or GUI dependency, so it is unit-testable in isolation.
class TrackSimulator {
public:
    explicit TrackSimulator(double areaRadiusKm = 40.0);

    void advance(double dtSeconds);

    [[nodiscard]] const std::vector<Track>& tracks() const { return m_tracks; }
    [[nodiscard]] double areaRadiusKm() const { return m_areaRadiusKm; }

private:
    void seedTracks();

    std::vector<Track> m_tracks;
    std::vector<double> m_turnRateDegPerSec;
    double m_areaRadiusKm;
};

} // namespace qttutorial::defence
