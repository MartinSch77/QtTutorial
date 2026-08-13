// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

#include <vector>

namespace qttutorial::defence {

// A track received over a simulated tactical data link from another
// platform - as opposed to a track this platform's own sensors pick up
// directly (see TrackSimulator). Same passive content as an organic track
// (position/heading/speed/domain classification) with no aim/intercept
// solution and no engagement data of any kind.
struct DataLinkTrack {
    int id = 0;
    QString classification;
    QString domain; // "Air" | "Surface" | "Ground"
    double xKm = 0.0;
    double yKm = 0.0;
    double headingDeg = 0.0;
    double speedKmh = 0.0;
    bool stale = false;
    double dataAgeSeconds = 0.0;
};

// Models a couple of off-board tracks whose *displayed* position only
// refreshes when a simulated tactical data link "delivers" an update. The
// delivery cadence is driven by a link-quality percentage (in practice fed
// from the same value that drives the Comms subsystem health-grid cell) so
// a degraded comms subsystem plausibly shows up here as intermittent,
// aging off-board track data. This is a data-quality effect only - it never
// changes what is shown about a track beyond position/heading/domain and a
// staleness indicator, and never computes anything about engaging it.
class DataLinkTrackModel {
public:
    explicit DataLinkTrackModel(double areaRadiusKm = 40.0);

    void advance(double dtSeconds, double linkQualityPercent);

    [[nodiscard]] const std::vector<DataLinkTrack>& tracks() const { return m_tracks; }
    [[nodiscard]] double areaRadiusKm() const { return m_areaRadiusKm; }

    // The delivery-cadence formula, exposed as a pure static function so it
    // is directly unit-testable without advancing the whole model.
    [[nodiscard]] static int updateIntervalTicks(double linkQualityPercent);

private:
    void seedTracks();

    std::vector<DataLinkTrack> m_tracks;
    std::vector<double> m_trueXKm;
    std::vector<double> m_trueYKm;
    std::vector<double> m_trueHeadingDeg;
    std::vector<double> m_turnRateDegPerSec;
    std::vector<int> m_ticksSinceUpdate;
    double m_areaRadiusKm;
};

} // namespace qttutorial::defence
