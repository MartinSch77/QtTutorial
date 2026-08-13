// SPDX-License-Identifier: MIT
#pragma once

#include "Asset.h"
#include "AlertLog.h"
#include "CommsLinkQualityModel.h"

#include <QObject>

#include <random>
#include <vector>

namespace qttutorial::defence {

// Simulates a small mix of assets (vehicles, drones, a vessel and a static
// installation) as if status reports were streaming in from the field: a
// gentle random-walk health metric per asset, with one asset seeded with a
// slow drift fault so the offboard alerting has something genuine to catch.
// Mobile assets move smoothly; the installation stays put. No weapons or
// targeting data of any kind is modeled.
class FleetSimulator : public QObject {
    Q_OBJECT
public:
    explicit FleetSimulator(QObject* parent = nullptr);

    void advance(double dtSeconds);

    [[nodiscard]] const std::vector<Asset>& assets() const { return m_assets; }

signals:
    void fleetUpdated();
    void alertRaised(const Alert& alert);

private:
    void seedFleet();
    [[nodiscard]] static QString classifyHealth(double value);
    [[nodiscard]] static QString classifyComms(double value);

    std::vector<Asset> m_assets;
    std::vector<double> m_healthValues;
    std::vector<double> m_healthDriftPerSecond;
    std::vector<double> m_speedKmh;

    // Comms-link simulation, correlated with (but independent of) overall
    // health: a random-walk quality value per asset, a per-asset "true"
    // position that always keeps moving, and a tick counter deciding when
    // the *displayed* position/telemetry actually refreshes. See
    // CommsLinkQualityModel for the delivery-cadence formula.
    std::vector<double> m_commsQuality;
    std::vector<double> m_commsDriftPerSecond;
    std::vector<QString> m_previousCommsHealth;
    std::vector<double> m_trueXKm;
    std::vector<double> m_trueYKm;
    std::vector<double> m_trueHeadingDeg;
    std::vector<int> m_ticksSinceUpdate;

    std::mt19937 m_rng{12345};
};

} // namespace qttutorial::defence
