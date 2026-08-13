// SPDX-License-Identifier: MIT
#pragma once

#include "ProcessPointSimulator.h"

namespace qttutorial::plant_scada {

enum class Severity { Normal, Warning, Critical };

// Classifies a reading against the tag's configured limits. Pure, so it is
// exercised directly by tests without needing a running historian.
[[nodiscard]] Severity evaluate(const TagDefinition& tag, double value);

[[nodiscard]] const char* severityLabel(Severity severity);

} // namespace qttutorial::plant_scada
