// SPDX-License-Identifier: MIT
#pragma once

#include "HaulHistoryStore.h"
#include "PitFleetModel.h"
#include "ProductionTrendWidget.h"

#include <QMainWindow>

#include <memory>

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

    PitFleetModel* m_model;
    QTableView* m_view;
    ProductionTrendWidget* m_trendWidget;
    QLabel* m_selectedTruckLabel;
    std::unique_ptr<HaulHistoryStore> m_historyStore;
};

} // namespace qttutorial::mining::pit
