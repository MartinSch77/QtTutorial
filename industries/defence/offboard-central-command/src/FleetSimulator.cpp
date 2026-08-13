// SPDX-License-Identifier: MIT
#include "FleetSimulator.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace qttutorial::defence {

namespace {
constexpr double kHealthNominal = 90.0;
constexpr double kHealthCautionBelow = 60.0;
constexpr double kHealthCriticalBelow = 35.0;
}

FleetSimulator::FleetSimulator(QObject* parent)
    : QObject(parent)
{
    seedFleet();
}

void FleetSimulator::seedFleet()
{
    const struct { const char* id; AssetType type; double x; double y; double heading; double speedKmh; } seeds[] = {
        {"VEH-01", AssetType::Vehicle, -30.0, 20.0, 90.0, 25.0},
        {"VEH-02", AssetType::Vehicle, 40.0, -15.0, 200.0, 22.0},
        {"DRN-01", AssetType::Drone, 10.0, 60.0, 270.0, 60.0},
        {"DRN-02", AssetType::Drone, -50.0, -40.0, 45.0, 55.0},
        {"VSL-01", AssetType::Vessel, 70.0, 10.0, 160.0, 18.0},
        {"INS-01", AssetType::Installation, 0.0, 0.0, 0.0, 0.0},
        {"INS-02", AssetType::Installation, -70.0, 55.0, 0.0, 0.0},
    };

    m_assets.clear();
    m_healthValues.clear();
    m_healthDriftPerSecond.clear();
    m_speedKmh.clear();

    int index = 0;
    for (const auto& seed : seeds) {
        Asset asset;
        asset.id = QString::fromLatin1(seed.id);
        asset.type = seed.type;
        asset.xKm = seed.x;
        asset.yKm = seed.y;
        asset.headingDeg = seed.heading;
        asset.health = QStringLiteral("Nominal");
        m_assets.push_back(asset);
        m_healthValues.push_back(kHealthNominal);
        m_speedKmh.push_back(seed.speedKmh);

        // VEH-02 gets a slow one-directional drift fault so the alert log has
        // something real to escalate through Caution into Critical.
        m_healthDriftPerSecond.push_back(index == 1 ? -0.5 : 0.0);
        ++index;
    }
}

QString FleetSimulator::classifyHealth(double value)
{
    if (value < kHealthCriticalBelow) {
        return QStringLiteral("Critical");
    }
    if (value < kHealthCautionBelow) {
        return QStringLiteral("Caution");
    }
    return QStringLiteral("Nominal");
}

void FleetSimulator::advance(double dtSeconds)
{
    std::normal_distribution<double> healthNoise(0.0, 0.4);
    const QDateTime now = QDateTime::currentDateTimeUtc();

    for (std::size_t i = 0; i < m_assets.size(); ++i) {
        Asset& asset = m_assets[i];

        if (m_speedKmh[i] > 0.0) {
            const double headingRad = asset.headingDeg * std::numbers::pi / 180.0;
            const double speedKmPerSec = m_speedKmh[i] / 3600.0;
            asset.xKm += std::cos(headingRad) * speedKmPerSec * dtSeconds;
            asset.yKm += std::sin(headingRad) * speedKmPerSec * dtSeconds;
        }

        const double pullToNominal = (kHealthNominal - m_healthValues[i]) * 0.01 * dtSeconds;
        const double drift = m_healthDriftPerSecond[i] * dtSeconds;
        m_healthValues[i] =
            std::clamp(m_healthValues[i] + pullToNominal + drift + healthNoise(m_rng) * dtSeconds, 0.0, 100.0);

        const QString previousHealth = asset.health;
        asset.health = classifyHealth(m_healthValues[i]);
        asset.lastUpdated = now;

        if (asset.health != previousHealth && asset.health != QStringLiteral("Nominal")) {
            Alert alert;
            alert.assetId = asset.id;
            alert.message = QStringLiteral("%1 health degraded to %2").arg(asset.id, asset.health);
            alert.severity = asset.health == QStringLiteral("Critical") ? AlertSeverity::Critical
                                                                          : AlertSeverity::Caution;
            alert.timestamp = now;
            emit alertRaised(alert);
        }
    }

    emit fleetUpdated();
}

} // namespace qttutorial::defence
