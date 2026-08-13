// SPDX-License-Identifier: MIT
#pragma once

#include "AlertLevel.h"

#include <QString>

#include <vector>

namespace qttutorial::startrek_bridge {

// A simulated externally-detected object shown on the tactical/science
// sensor-scan readout: distance and bearing only. This is fictional/cosmetic
// flavour data, not a real object-detection or targeting system.
struct SensorContact {
    int id = 0;
    QString label;
    double distanceKm = 0.0;
    double bearingDeg = 0.0;
};

// Drives the Tactical/Science station's simulated EXTERNAL condition data:
// ambient radiation, hull stress from the exterior environment, a purely
// cosmetic "shield strength" percentage, and a handful of SensorContacts.
// Shields ease toward a high target under Yellow/Red alert and toward a low
// idle baseline under Green (raising/lowering shields), rather than
// jumping. Pure C++23, deterministic given elapsed time and the current
// AlertLevel.
class TacticalSimulator {
public:
    TacticalSimulator();

    void advance(double dtSeconds, AlertLevel alertLevel);

    [[nodiscard]] double externalRadiationLevel() const { return m_externalRadiationLevel; }
    [[nodiscard]] double hullStressLevel() const { return m_hullStressLevel; }
    [[nodiscard]] double shieldStrengthPercent() const { return m_shieldStrengthPercent; }
    [[nodiscard]] const std::vector<SensorContact>& contacts() const { return m_contacts; }

private:
    void seedContacts();

    double m_externalRadiationLevel = 0.2;
    double m_hullStressLevel = 5.0;
    double m_shieldStrengthPercent = 15.0;
    std::vector<SensorContact> m_contacts;
    std::vector<double> m_distanceDriftKmPerMin;
    std::vector<double> m_bearingDriftDegPerSec;
    double m_elapsedSeconds = 0.0;
};

} // namespace qttutorial::startrek_bridge
