// SPDX-License-Identifier: MIT
#pragma once

#include "HaulHistoryStore.h"
#include "PitFleetModel.h"
#include "PitMapWidget.h"
#include "ProductionTrendWidget.h"

#include <QMainWindow>

#include <memory>
#include <vector>

class QTableView;
class QLabel;

namespace qttutorial::mining::pit {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void onSamplesUpdated();
    void onSelectionChanged();
    void refreshTrend();
    void refreshFleetTrend();

    PitFleetModel* m_model;
    QTableView* m_view;
    ProductionTrendWidget* m_trendWidget;
    ProductionTrendWidget* m_fleetTrendWidget;
    PitMapWidget* m_pitMapWidget;
    QLabel* m_selectedTruckLabel;
    QLabel* m_fleetTonnesLabel;
    std::unique_ptr<HaulHistoryStore> m_historyStore;
    std::vector<double> m_fleetTonnesHistory;
};

} // namespace qttutorial::mining::pit
