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

constexpr double kCommsNominal = 92.0;
constexpr double kCommsCautionBelow = 55.0;
constexpr double kCommsCriticalBelow = 30.0;
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
    m_commsQuality.clear();
    m_commsDriftPerSecond.clear();
    m_previousCommsHealth.clear();
    m_trueXKm.clear();
    m_trueYKm.clear();
    m_trueHeadingDeg.clear();
    m_ticksSinceUpdate.clear();

    int index = 0;
    for (const auto& seed : seeds) {
        Asset asset;
        asset.id = QString::fromLatin1(seed.id);
        asset.type = seed.type;
        asset.xKm = seed.x;
        asset.yKm = seed.y;
        asset.headingDeg = seed.heading;
        asset.health = QStringLiteral("Nominal");
        asset.commsQualityPercent = kCommsNominal;
        m_assets.push_back(asset);
        m_healthValues.push_back(kHealthNominal);
        m_speedKmh.push_back(seed.speedKmh);

        // VEH-02 gets a slow one-directional health drift fault so the alert
        // log has something real to escalate through Caution into Critical.
        m_healthDriftPerSecond.push_back(index == 1 ? -0.5 : 0.0);

        // DRN-02 separately gets a comms-link drift fault: a plausible,
        // independent failure mode where the platform itself stays healthy
        // but its data link degrades, so its track on the tactical map goes
        // stale/intermittent rather than its health tile turning red.
        m_commsQuality.push_back(kCommsNominal);
        m_commsDriftPerSecond.push_back(index == 3 ? -0.55 : 0.0);
        m_previousCommsHealth.push_back(QStringLiteral("Nominal"));

        m_trueXKm.push_back(seed.x);
        m_trueYKm.push_back(seed.y);
        m_trueHeadingDeg.push_back(seed.heading);
        m_ticksSinceUpdate.push_back(0);

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

QString FleetSimulator::classifyComms(double value)
{
    if (value < kCommsCriticalBelow) {
        return QStringLiteral("Critical");
    }
    if (value < kCommsCautionBelow) {
        return QStringLiteral("Caution");
    }
    return QStringLiteral("Nominal");
}

void FleetSimulator::advance(double dtSeconds)
{
    std::normal_distribution<double> healthNoise(0.0, 0.4);
    std::normal_distribution<double> commsNoise(0.0, 0.5);
    const QDateTime now = QDateTime::currentDateTimeUtc();

    for (std::size_t i = 0; i < m_assets.size(); ++i) {
        Asset& asset = m_assets[i];

        // The "true" kinematic state always advances, exactly as before -
        // a comms outage means we stop *hearing about* the platform's
        // position, not that the platform itself stops moving.
        if (m_speedKmh[i] > 0.0) {
            const double headingRad = m_trueHeadingDeg[i] * std::numbers::pi / 180.0;
            const double speedKmPerSec = m_speedKmh[i] / 3600.0;
            m_trueXKm[i] += std::cos(headingRad) * speedKmPerSec * dtSeconds;
            m_trueYKm[i] += std::sin(headingRad) * speedKmPerSec * dtSeconds;
        }

        const double pullToNominal = (kHealthNominal - m_healthValues[i]) * 0.01 * dtSeconds;
        const double drift = m_healthDriftPerSecond[i] * dtSeconds;
        m_healthValues[i] =
            std::clamp(m_healthValues[i] + pullToNominal + drift + healthNoise(m_rng) * dtSeconds, 0.0, 100.0);

        const double commsPull = (kCommsNominal - m_commsQuality[i]) * 0.01 * dtSeconds;
        const double commsDrift = m_commsDriftPerSecond[i] * dtSeconds;
        m_commsQuality[i] =
            std::clamp(m_commsQuality[i] + commsPull + commsDrift + commsNoise(m_rng) * dtSeconds, 0.0, 100.0);
        asset.commsQualityPercent = m_commsQuality[i];

        const QString previousHealth = asset.health;
        asset.health = classifyHealth(m_healthValues[i]);

        if (asset.health != previousHealth && asset.health != QStringLiteral("Nominal")) {
            Alert alert;
            alert.assetId = asset.id;
            alert.message = QStringLiteral("%1 health degraded to %2").arg(asset.id, asset.health);
            alert.severity = asset.health == QStringLiteral("Critical") ? AlertSeverity::Critical
                                                                          : AlertSeverity::Caution;
            alert.timestamp = now;
            emit alertRaised(alert);
        }

        const QString commsHealth = classifyComms(m_commsQuality[i]);
        if (commsHealth != m_previousCommsHealth[i] && commsHealth != QStringLiteral("Nominal")) {
            Alert alert;
            alert.assetId = asset.id;
            alert.message = QStringLiteral("%1 comms link degraded to %2 - track updates may go stale")
                                     .arg(asset.id, commsHealth);
            alert.severity =
                commsHealth == QStringLiteral("Critical") ? AlertSeverity::Critical : AlertSeverity::Caution;
            alert.timestamp = now;
            emit alertRaised(alert);
        }
        m_previousCommsHealth[i] = commsHealth;

        // The *displayed* position/heading only refreshes when the
        // simulated comms link "delivers" this tick; otherwise it stays
        // frozen at the last-known-good value and ages, which is exactly
        // what a real command-center track display shows during a comms
        // outage: a stale plot, not a moving one.
        const int interval = commsLinkUpdateIntervalTicks(m_commsQuality[i]);
        ++m_ticksSinceUpdate[i];
        if (m_ticksSinceUpdate[i] >= interval) {
            asset.xKm = m_trueXKm[i];
            asset.yKm = m_trueYKm[i];
            asset.headingDeg = m_trueHeadingDeg[i];
            asset.lastUpdated = now;
            asset.trackStale = false;
            asset.dataAgeSeconds = 0.0;
            m_ticksSinceUpdate[i] = 0;
        } else {
            asset.trackStale = true;
            asset.dataAgeSeconds += dtSeconds;
        }
    }

    emit fleetUpdated();
}

} // namespace qttutorial::defence
