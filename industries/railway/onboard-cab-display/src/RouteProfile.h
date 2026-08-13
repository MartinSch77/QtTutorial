// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

#include <optional>
#include <vector>

namespace qttutorial::cab_display {

struct SpeedSegment {
    double startDistanceM = 0.0;
    double endDistanceM = 0.0;
    double permittedSpeedKmh = 0.0;
};

struct Station {
    QString name;
    double distanceM = 0.0;
};

struct Restriction {
    double distanceToStartM = 0.0;
    double permittedSpeedKmh = 0.0;
};

// A route as a sequence of speed-restriction segments plus station stops, so
// the speed and restriction values the cab display shows are geometrically
// derived from a single, consistent model instead of picked independently.
class RouteProfile {
public:
    explicit RouteProfile(std::vector<SpeedSegment> segments, std::vector<Station> stations = {});

    [[nodiscard]] double permittedSpeedAt(double distanceM) const;

    // The next segment strictly ahead whose permitted speed is lower than
    // the current one, if any — the basis for the driving-advice margin bar.
    [[nodiscard]] std::optional<Restriction> nextRestriction(double distanceM) const;

    [[nodiscard]] std::optional<Station> nextStation(double distanceM) const;

    [[nodiscard]] double totalLength() const;

private:
    std::vector<SpeedSegment> m_segments;
    std::vector<Station> m_stations;
};

} // namespace qttutorial::cab_display
