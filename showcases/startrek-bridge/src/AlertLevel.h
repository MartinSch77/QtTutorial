// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

namespace qttutorial::startrek_bridge {

// The ship-wide "condition" shared by every station: Green (normal
// operations), Yellow (heightened readiness) and Red (combat/emergency
// footing). This is the single piece of state that visibly ties every
// panel together - switching it anywhere is reflected everywhere.
enum class AlertLevel { Green, Yellow, Red };

[[nodiscard]] QString alertLevelName(AlertLevel level);

} // namespace qttutorial::startrek_bridge
