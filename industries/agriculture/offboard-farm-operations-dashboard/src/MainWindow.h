// SPDX-License-Identifier: MIT
#pragma once

#include "FieldMapWidget.h"
#include "FieldOperationsModel.h"
#include "OperationHistoryStore.h"

#include <QMainWindow>

#include <memory>

class QTableView;
class QLabel;

namespace qttutorial::agriculture::ops {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void onSamplesUpdated();
    void onSelectionChanged();
    void refreshMap();

    FieldOperationsModel* m_model;
    QTableView* m_view;
    FieldMapWidget* m_mapWidget;
    QLabel* m_selectedFieldLabel;
    std::unique_ptr<OperationHistoryStore> m_historyStore;
};

} // namespace qttutorial::agriculture::ops
