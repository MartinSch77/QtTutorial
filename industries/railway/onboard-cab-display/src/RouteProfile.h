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
    // Track gradient as a percentage; positive = climbing, negative =
    // descending. The braking-curve function deliberately assumes a flat
    // track (a real ETCS driving-advice curve is itself a simplification),
    // so a steep downhill segment is where the simulator's actual achievable
    // deceleration can genuinely fall short of what the curve assumed —
    // the realistic condition the brake-warning indication exists to catch.
    double gradientPercent = 0.0;
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

    // The gradient (percent, positive = uphill) of the segment covering
    // `distanceM`, for the simulator's physically-aware speed integration.
    [[nodiscard]] double gradientPercentAt(double distanceM) const;

    // The next segment strictly ahead whose permitted speed is lower than
    // the current one, if any — the basis for the driving-advice margin bar.
    [[nodiscard]] std::optional<Restriction> nextRestriction(double distanceM) const;

    // Every speed *drop* (not every segment boundary) within `lookaheadM` of
    // `distanceM`, in order, capped at `maxCount` — the basis for the DMI-style
    // "planning area" strip that shows a driver more than just the single
    // next restriction. A drop is a segment whose permitted speed is lower
    // than the segment immediately preceding it, so a strip of segments that
    // merely return to the current speed after a dip does not appear twice.
    [[nodiscard]] std::vector<Restriction> upcomingRestrictions(double distanceM, double lookaheadM,
                                                                 std::size_t maxCount = 3) const;

    [[nodiscard]] std::optional<Station> nextStation(double distanceM) const;

    [[nodiscard]] double totalLength() const;

private:
    std::vector<SpeedSegment> m_segments;
    std::vector<Station> m_stations;
};

} // namespace qttutorial::cab_display
