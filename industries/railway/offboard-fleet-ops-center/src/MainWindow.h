// SPDX-License-Identifier: MIT
#pragma once

#include "DelayTrendWidget.h"
#include "FleetMapWidget.h"
#include "FleetSimulator.h"
#include "FleetTableModel.h"
#include "RunHistoryStore.h"

#include <QDateTime>
#include <QMainWindow>
#include <QTimer>

class QTableView;
class QComboBox;

namespace qttutorial::fleet_ops {

// The fleet operations centre dashboard: a schematic fleet map, a live table
// of every train on the route, a delay/punctuality trend for whichever
// train is selected, a fleet-wide punctuality trend, and a SQLite-persisted
// run history behind all three.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onTick();
    void onTrainSelectionChanged(int row);

private:
    void refreshTrend();
    void refreshNetworkTrend();

    FleetConfig m_config;
    RunHistoryStore m_history;
    FleetMapWidget* m_map;
    FleetTableModel* m_model;
    QTableView* m_table;
    DelayTrendWidget* m_trend;
    DelayTrendWidget* m_networkTrend;
    QComboBox* m_trainSelector;
    QTimer m_timer;
    QDateTime m_simulationStart;
    int m_selectedTrainIndex = 0;
};

} // namespace qttutorial::fleet_ops
