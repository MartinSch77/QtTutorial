// SPDX-License-Identifier: MIT
#pragma once

#include "Asset.h"
#include "AlertLog.h"

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

    std::vector<Asset> m_assets;
    std::vector<double> m_healthValues;
    std::vector<double> m_healthDriftPerSecond;
    std::vector<double> m_speedKmh;
    std::mt19937 m_rng{12345};
};

} // namespace qttutorial::defence
