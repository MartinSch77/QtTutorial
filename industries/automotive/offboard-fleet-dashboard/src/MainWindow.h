// SPDX-License-Identifier: MIT
#pragma once

#include "FleetModel.h"
#include "TelemetryHistoryStore.h"
#include "TrendWidget.h"

#include <QMainWindow>

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

    FleetModel* m_model;
    QTableView* m_view;
    TrendWidget* m_trendWidget;
    QLabel* m_selectedVehicleLabel;
    std::unique_ptr<TelemetryHistoryStore> m_historyStore;
};

} // namespace qttutorial::automotive::fleet
