// SPDX-License-Identifier: MIT
#include "TacticalSimulator.h"

#include <algorithm>
#include <cmath>

namespace qttutorial::startrek_bridge {

namespace {
double clamp(double value, double low, double high)
{
    return std::max(low, std::min(high, value));
}

double normalizeDeg(double deg)
{
    double result = std::fmod(deg, 360.0);
    if (result < 0.0) {
        result += 360.0;
    }
    return result;
}

constexpr double kMinContactDistanceKm = 4.0;
constexpr double kMaxContactDistanceKm = 180.0;
}

TacticalSimulator::TacticalSimulator()
{
    seedContacts();
}

void TacticalSimulator::seedContacts()
{
    const struct { const char* label; double distanceKm; double bearingDeg; double distanceDriftKmPerMin;
                   double bearingDriftDegPerSec; } seeds[] = {
        {"Unidentified contact", 62.0, 45.0, -1.2, 1.5},
        {"Ion trail", 130.0, 190.0, 0.8, -0.6},
        {"Debris cluster", 24.0, 300.0, -0.3, 2.1},
    };

    m_contacts.clear();
    m_distanceDriftKmPerMin.clear();
    m_bearingDriftDegPerSec.clear();
    int id = 1;
    for (const auto& seed : seeds) {
        SensorContact contact;
        contact.id = id++;
        contact.label = QString::fromLatin1(seed.label);
        contact.distanceKm = seed.distanceKm;
        contact.bearingDeg = seed.bearingDeg;
        m_contacts.push_back(contact);
        m_distanceDriftKmPerMin.push_back(seed.distanceDriftKmPerMin);
        m_bearingDriftDegPerSec.push_back(seed.bearingDriftDegPerSec);
    }
}

void TacticalSimulator::advance(double dtSeconds, AlertLevel alertLevel)
{
    m_elapsedSeconds += dtSeconds;

    const double redBoost = alertLevel == AlertLevel::Red ? 0.6 : 0.0;
    m_externalRadiationLevel = clamp(0.4 + 0.3 * std::sin(m_elapsedSeconds * 0.09) + redBoost, 0.0, 5.0);
    m_hullStressLevel = clamp(8.0 + 6.0 * std::sin(m_elapsedSeconds * 0.06 + 0.5) + redBoost * 10.0, 0.0, 100.0);

    const double shieldTarget = alertLevel == AlertLevel::Green ? 15.0 : (alertLevel == AlertLevel::Yellow ? 70.0 : 98.0);
    m_shieldStrengthPercent += (shieldTarget - m_shieldStrengthPercent) * std::min(1.0, dtSeconds * 0.6);
    m_shieldStrengthPercent = clamp(m_shieldStrengthPercent, 0.0, 100.0);

    for (std::size_t i = 0; i < m_contacts.size(); ++i) {
        SensorContact& contact = m_contacts[i];
        contact.bearingDeg = normalizeDeg(contact.bearingDeg + m_bearingDriftDegPerSec[i] * dtSeconds);

        double newDistance = contact.distanceKm + m_distanceDriftKmPerMin[i] / 60.0 * dtSeconds;
        if (newDistance < kMinContactDistanceKm || newDistance > kMaxContactDistanceKm) {
            m_distanceDriftKmPerMin[i] = -m_distanceDriftKmPerMin[i];
            newDistance = clamp(newDistance, kMinContactDistanceKm, kMaxContactDistanceKm);
        }
        contact.distanceKm = newDistance;
    }
}

} // namespace qttutorial::startrek_bridge
