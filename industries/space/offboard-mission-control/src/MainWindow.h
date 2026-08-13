// SPDX-License-Identifier: MIT
#pragma once

#include "FleetOrbitSimulator.h"
#include "GroundStationTracker.h"
#include "TelemetryHistoryStore.h"

#include <QTimer>
#include <QWidget>

#include <memory>

class QLabel;
class QTableView;

namespace qttutorial::space {

class SatelliteTableModel;
class BatteryTrendWidget;
class WorldMapWidget;
class PassScheduleWidget;

// The mission-control counterpart to the onboard satellite telemetry app: a
// fleet table, a QPainter-drawn battery trend for the selected satellite, and
// SQLite-backed telemetry history queried with parameterized SQL.
class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void onTick();
    void onSelectionChanged();

    FleetOrbitSimulator m_simulator;
    GroundStationTracker m_stationTracker;
    std::unique_ptr<TelemetryHistoryStore> m_history;
    SatelliteTableModel* m_fleetModel;
    QTableView* m_fleetView;
    BatteryTrendWidget* m_trendWidget;
    WorldMapWidget* m_worldMap;
    PassScheduleWidget* m_passSchedule;
    QLabel* m_selectedLabel;
    QTimer* m_timer;
};

} // namespace qttutorial::space
