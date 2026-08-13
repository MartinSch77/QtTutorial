// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QString>

#include <vector>

namespace qttutorial::graphs_showcase {

// Pure, deterministic simulated telemetry source: three sensors sampled as a
// function of elapsed time. No QObject, no randomness, so it is testable
// independently of the QTimer/QML machinery that drives it at run time.
class TelemetryGenerator {
public:
    [[nodiscard]] static std::vector<QString> sensorNames();

    [[nodiscard]] static double valueAt(int sensorIndex, double elapsedSeconds);
};

} // namespace qttutorial::graphs_showcase
