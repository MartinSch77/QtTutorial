// SPDX-License-Identifier: MIT
#pragma once

#include "FleetModel.h"
#include "TelemetryHistoryStore.h"
#include "TrendWidget.h"

#include <QMainWindow>

#include <deque>
#include <memory>

class QTableView;
class QLabel;

namespace qttutorial::automotive::fleet {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void onSamplesUpdated();
    void onSelectionChanged();
    void refreshTrend();
    void refreshFleetTrend();

    FleetModel* m_model;
    QTableView* m_view;
    TrendWidget* m_trendWidget;
    TrendWidget* m_fleetTrendWidget;
    QLabel* m_selectedVehicleLabel;
    QLabel* m_fleetTrendLabel;
    std::unique_ptr<TelemetryHistoryStore> m_historyStore;
    std::deque<double> m_fleetEfficiencyHistory;
};

} // namespace qttutorial::automotive::fleet
