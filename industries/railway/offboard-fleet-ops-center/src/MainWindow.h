// SPDX-License-Identifier: MIT
#pragma once

#include "DelayTrendWidget.h"
#include "FleetSimulator.h"
#include "FleetTableModel.h"
#include "RunHistoryStore.h"

#include <QDateTime>
#include <QMainWindow>
#include <QTimer>

class QTableView;
class QComboBox;

namespace qttutorial::fleet_ops {

// The fleet operations centre dashboard: a live table of every train on the
// route, a delay/punctuality trend for whichever train is selected, and a
// SQLite-persisted run history behind both.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onTick();
    void onTrainSelectionChanged(int row);

private:
    void refreshTrend();

    FleetConfig m_config;
    RunHistoryStore m_history;
    FleetTableModel* m_model;
    QTableView* m_table;
    DelayTrendWidget* m_trend;
    QComboBox* m_trainSelector;
    QTimer m_timer;
    QDateTime m_simulationStart;
    int m_selectedTrainIndex = 0;
};

} // namespace qttutorial::fleet_ops
