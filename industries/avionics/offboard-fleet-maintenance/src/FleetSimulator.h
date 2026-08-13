// SPDX-License-Identifier: MIT
#pragma once

#include "EngineTrend.h"
#include "Fleet.h"
#include "InspectionScheduler.h"

#include <QObject>

#include <random>
#include <vector>

namespace qttutorial::avionics {

// Simulates a small fleet's engine parameters as if telemetry were streaming in
// from line stations: mostly gentle random-walk noise around a nominal value,
// with one or two aircraft seeded with a slow one-directional drift fault so the
// predictive-maintenance flag (backed by EngineTrend) has something real to catch.
// It also accumulates each aircraft's utilization (flight hours/cycles since its
// last inspection) at a demo-accelerated rate, and uses InspectionScheduler to
// correlate "close to due" utilization with an active engine anomaly into an
// escalated, urgent maintenance task.
class FleetSimulator : public QObject {
    Q_OBJECT
public:
    explicit FleetSimulator(QObject* parent = nullptr);

    void advance(double dtSeconds);

    [[nodiscard]] const std::vector<Aircraft>& aircraft() const { return m_aircraft; }
    [[nodiscard]] const std::vector<MaintenanceTask>& tasks() const { return m_tasks; }
    [[nodiscard]] InspectionStatus inspectionStatus(std::size_t aircraftIndex) const;

signals:
    void fleetUpdated();
    void maintenanceTaskCreated(const MaintenanceTask& task);

private:
    void seedFleet();
    double stepParameter(double current, double nominal, double driftPerSecond, double dtSeconds,
                          std::normal_distribution<double>& noise);

    std::vector<Aircraft> m_aircraft;
    std::vector<MaintenanceTask> m_tasks;
    std::vector<EngineTrend> m_vibrationTrends;
    std::vector<double> m_vibrationDriftPerSecond;
    std::vector<bool> m_wasUrgent;
    std::vector<double> m_cyclesFractional;
    InspectionScheduler m_inspectionScheduler;
    std::mt19937 m_rng{12345};
    int m_nextTaskId = 1;
};

} // namespace qttutorial::avionics
