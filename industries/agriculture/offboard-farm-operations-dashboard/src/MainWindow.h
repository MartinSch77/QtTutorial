// SPDX-License-Identifier: MIT
#pragma once

#include "FieldMapWidget.h"
#include "FieldOperationsModel.h"
#include "FieldStatusOverviewWidget.h"
#include "OperationHistoryStore.h"
#include "TrendChartWidget.h"

#include <QMainWindow>

#include <memory>

class QTableView;
class QLabel;
class QTabWidget;

namespace qttutorial::agriculture::ops {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void onSamplesUpdated();
    void onSelectionChanged();
    void refreshMap();
    void refreshTrend();

    FieldOperationsModel* m_model;
    QTableView* m_view;
    FieldStatusOverviewWidget* m_overviewWidget;
    QTabWidget* m_detailTabs;
    FieldMapWidget* m_mapWidget;
    TrendChartWidget* m_fuelTrendWidget;
    QLabel* m_selectedFieldLabel;
    std::unique_ptr<OperationHistoryStore> m_historyStore;
};

} // namespace qttutorial::agriculture::ops
