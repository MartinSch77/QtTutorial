// SPDX-License-Identifier: MIT
#pragma once

#include "Protocol.h"

#include <QStringList>

namespace qttutorial::homeautomation::remote {

// Compares two snapshots and produces one human-readable description per
// meaningful device-state change (e.g. "Living Room lights turned on"),
// mirroring what a real remote-access app's activity log would show. Sensor
// triggers and the thermostat's slowly-drifting current temperature are
// intentionally excluded - they are not "changes a user made", they would
// flood the log every tick.
[[nodiscard]] QStringList describeChanges(const Snapshot& previous, const Snapshot& next);

} // namespace qttutorial::homeautomation::remote
