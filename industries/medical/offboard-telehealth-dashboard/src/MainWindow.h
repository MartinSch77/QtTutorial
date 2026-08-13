// SPDX-License-Identifier: MIT
#pragma once

#include "PatientListModel.h"
#include "VitalsHistoryStore.h"

#include <QMainWindow>

#include <memory>

class QListView;
class QListWidget;

namespace qttutorial::medical::telehealth {

class PatientOverviewWidget;
class TrendChartWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void onSamplesUpdated();
    void refreshAlerts();
    void onPatientSelected(const QString& patientId, const QString& name);

    PatientListModel* m_model;
    PatientOverviewWidget* m_overview;
    QListView* m_patientList;
    QListWidget* m_alertList;
    TrendChartWidget* m_trendChart;
    std::unique_ptr<VitalsHistoryStore> m_historyStore;
};

} // namespace qttutorial::medical::telehealth
