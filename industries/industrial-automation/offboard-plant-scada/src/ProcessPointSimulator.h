// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

#include <vector>

namespace qttutorial::plant_scada {

// Describes one simulated process point (a SCADA "tag"): a believable signal
// built from a slow drift plus a periodic component, not unstructured noise.
struct TagDefinition {
    QString tagId;
    QString unit;
    double baseValue = 0.0;
    double amplitude = 0.0;
    double periodSeconds = 60.0;
    double driftPerHour = 0.0;
    double warnLow = -1e9;
    double warnHigh = 1e9;
    double criticalLow = -1e9;
    double criticalHigh = 1e9;
};

[[nodiscard]] const std::vector<TagDefinition>& defaultTags();

// Pure function of elapsed simulation time: same tSeconds always yields the
// same value, which is what makes the signal shape testable without mocking
// a clock.
[[nodiscard]] double valueAt(const TagDefinition& tag, double tSeconds);

} // namespace qttutorial::plant_scada
