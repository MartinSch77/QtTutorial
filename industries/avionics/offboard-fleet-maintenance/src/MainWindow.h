// SPDX-License-Identifier: MIT
#pragma once

#include "FleetSimulator.h"
#include "FlightHistoryStore.h"

#include <QTimer>
#include <QWidget>

#include <memory>

class QListWidget;
class QTableView;
class QLabel;

namespace qttutorial::avionics {

class FleetTableModel;
class TrendWidget;

// The back-office counterpart to the onboard PFD: an operations dashboard that
// aggregates simulated fleet telemetry, surfaces predictive-maintenance flags,
// and persists parameter history to SQLite via QtSql.
class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void onTick();
    void onSelectionChanged();
    void onMaintenanceTaskCreated(const MaintenanceTask& task);

    FleetSimulator m_simulator;
    std::unique_ptr<FlightHistoryStore> m_history;
    FleetTableModel* m_fleetModel;
    QTableView* m_fleetView;
    QListWidget* m_taskList;
    TrendWidget* m_trendWidget;
    QLabel* m_selectedLabel;
    QTimer* m_timer;
};

} // namespace qttutorial::avionics
