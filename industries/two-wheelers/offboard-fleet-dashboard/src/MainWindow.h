// SPDX-License-Identifier: MIT
#pragma once

#include "FleetAnalytics.h"
#include "FleetModel.h"
#include "TelemetryHistoryStore.h"
#include "TrendWidget.h"

#include <QMainWindow>

#include <memory>

class QTableView;
class QLabel;

namespace qttutorial::two_wheelers::fleet {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void onSamplesUpdated();
    void onSelectionChanged();
    void refreshTrend();
    void refreshFleetUtilizationTrend();

    FleetModel* m_model;
    QTableView* m_view;
    TrendWidget* m_trendWidget;
    QLabel* m_selectedVehicleLabel;
    TrendWidget* m_utilizationTrendWidget;
    QLabel* m_utilizationLabel;
    FleetUtilizationHistory m_utilizationHistory;
    std::unique_ptr<TelemetryHistoryStore> m_historyStore;
};

} // namespace qttutorial::two_wheelers::fleet
