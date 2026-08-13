// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

#include <vector>

namespace qttutorial::plant_scada {

// Describes one simulated process point (a SCADA "tag"): a believable signal
// built from a slow drift plus a periodic component, not unstructured noise.
// Tags are grouped into plant `line`s (cells) for the plant-wide overview;
// a tag with `gatedByLineRunning` set (a flow or conveyor/motor-speed style
// point) plausibly falls to near zero whenever its line is stopped, rather
// than continuing to oscillate as if equipment kept running with nothing
// feeding it.
struct TagDefinition {
    QString tagId;
    QString unit;
    QString line;
    double baseValue = 0.0;
    double amplitude = 0.0;
    double periodSeconds = 60.0;
    double driftPerHour = 0.0;
    double warnLow = -1e9;
    double warnHigh = 1e9;
    double criticalLow = -1e9;
    double criticalHigh = 1e9;
    bool gatedByLineRunning = false;
};

[[nodiscard]] const std::vector<TagDefinition>& defaultTags();

// Every distinct TagDefinition::line, in the order tags first introduce them.
[[nodiscard]] std::vector<QString> lineNames();

// Pure function of elapsed simulation time: a deterministic run/stop duty
// cycle for a given plant line, staggered per line (via a hash of its name)
// so lines do not all start and stop in lockstep. Used to gate flow/speed
// tags belonging to that line, and to report per-line running status on the
// plant overview.
[[nodiscard]] bool lineRunningAt(const QString& line, double tSeconds);

// Pure function of elapsed simulation time: same tSeconds always yields the
// same value, which is what makes the signal shape testable without mocking
// a clock. Tags marked gatedByLineRunning fall back to a small residual
// value while their line is stopped instead of the normal oscillation.
[[nodiscard]] double valueAt(const TagDefinition& tag, double tSeconds);

} // namespace qttutorial::plant_scada
