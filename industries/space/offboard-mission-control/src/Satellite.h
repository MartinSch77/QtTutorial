// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

namespace qttutorial::space {

enum class HealthSummary { Nominal, Caution, Critical };

struct SatelliteState {
    QString name;
    double phaseDeg = 0.0;
    double altitudeKm = 550.0;
    double batteryPercent = 85.0;
    bool inEclipse = false;
    HealthSummary health = HealthSummary::Nominal;
};

// Pure classification: combines battery level and eclipse duration risk into a
// single summary a mission-control operator can scan at a glance across an
// entire fleet table.
[[nodiscard]] HealthSummary classifyHealth(double batteryPercent, bool inEclipse);

[[nodiscard]] QString toString(HealthSummary health);

} // namespace qttutorial::space
