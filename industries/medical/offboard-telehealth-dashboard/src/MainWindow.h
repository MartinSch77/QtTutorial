// SPDX-License-Identifier: MIT
#pragma once

#include "PatientListModel.h"
#include "VitalsHistoryStore.h"

#include <QMainWindow>

#include <memory>

class QListView;
class QListWidget;

namespace qttutorial::medical::telehealth {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void onSamplesUpdated();
    void refreshAlerts();

    PatientListModel* m_model;
    QListView* m_patientList;
    QListWidget* m_alertList;
    std::unique_ptr<VitalsHistoryStore> m_historyStore;
};

} // namespace qttutorial::medical::telehealth
