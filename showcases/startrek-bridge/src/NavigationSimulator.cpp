// SPDX-License-Identifier: MIT
#include "NavigationSimulator.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace qttutorial::startrek_bridge {

namespace {
double normalizeDeg(double deg)
{
    double result = std::fmod(deg, 360.0);
    if (result < 0.0) {
        result += 360.0;
    }
    return result;
}

double clamp(double value, double low, double high)
{
    return std::max(low, std::min(high, value));
}
}

NavigationSimulator::NavigationSimulator(double areaRadiusKm)
    : m_areaRadiusKm(areaRadiusKm)
{
    seedContacts();
}

void NavigationSimulator::seedContacts()
{
    const struct {
        const char* label;
        double xKm;
        double yKm;
        double headingDeg;
        double speedKmPerMin;
        double turnRateDegPerSec;
    } seeds[] = {
        {"Unknown vessel", -30.0, 12.0, 95.0, 4.0, 0.6},
        {"Asteroid", 18.0, -22.0, 210.0, 1.2, 0.0},
        {"Planetoid", -10.0, -35.0, 60.0, 0.4, -0.1},
        {"Debris field", 34.0, 20.0, 275.0, 1.8, 1.4},
        {"Freighter", 5.0, 40.0, 190.0, 2.6, -0.5},
    };

    m_contacts.clear();
    m_turnRateDegPerSec.clear();
    int id = 1;
    for (const auto& seed : seeds) {
        NavContact contact;
        contact.id = id++;
        contact.label = QString::fromLatin1(seed.label);
        contact.xKm = seed.xKm;
        contact.yKm = seed.yKm;
        contact.headingDeg = seed.headingDeg;
        contact.speedKmPerMin = seed.speedKmPerMin;
        m_contacts.push_back(contact);
        m_turnRateDegPerSec.push_back(seed.turnRateDegPerSec);
    }
}

void NavigationSimulator::advance(double dtSeconds, AlertLevel alertLevel)
{
    m_elapsedSeconds += dtSeconds;

    m_headingDeg = normalizeDeg(m_headingDeg + 3.0 * dtSeconds);

    const double redBoost = alertLevel == AlertLevel::Red ? 3.0 : 0.0;
    m_warpFactor = clamp(3.4 + 2.3 * std::sin(m_elapsedSeconds * 0.05) + redBoost, 0.0, 9.9);

    const double impulseDamp = alertLevel == AlertLevel::Red ? -10.0 : 0.0;
    m_impulsePercent = clamp(28.0 + 14.0 * std::sin(m_elapsedSeconds * 0.08 + 1.0) + impulseDamp, 0.0, 100.0);

    for (std::size_t i = 0; i < m_contacts.size(); ++i) {
        NavContact& contact = m_contacts[i];
        contact.headingDeg = normalizeDeg(contact.headingDeg + m_turnRateDegPerSec[i] * dtSeconds);

        const double headingRad = contact.headingDeg * std::numbers::pi / 180.0;
        const double speedKmPerSec = contact.speedKmPerMin / 60.0;
        double newX = contact.xKm + std::cos(headingRad) * speedKmPerSec * dtSeconds;
        double newY = contact.yKm + std::sin(headingRad) * speedKmPerSec * dtSeconds;

        const double radius = std::hypot(newX, newY);
        if (radius > m_areaRadiusKm) {
            const double scale = m_areaRadiusKm / radius;
            newX *= scale;
            newY *= scale;
            contact.headingDeg = normalizeDeg(std::atan2(-newY, -newX) * 180.0 / std::numbers::pi);
        }

        contact.xKm = newX;
        contact.yKm = newY;
    }
}

} // namespace qttutorial::startrek_bridge
