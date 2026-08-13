// SPDX-License-Identifier: MIT
#pragma once

#include "Historian.h"
#include "TagTableModel.h"
#include "TrendWidget.h"

#include <QDateTime>
#include <QMainWindow>
#include <QTimer>

class QTableView;
class QListWidget;
class QComboBox;

namespace qttutorial::plant_scada {

// The control-room historian dashboard: a live tag table, a QPainter trend
// for whichever tag is selected, and an alarm history log, all backed by a
// SQLite-persisted historian rather than an in-memory list.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onTick();
    void onTagSelectionChanged(int row);

private:
    void refreshTrend();

    Historian m_historian;
    TagTableModel* m_tagModel;
    QTableView* m_tagTable;
    TrendWidget* m_trend;
    QListWidget* m_alarmList;
    QComboBox* m_tagSelector;
    QTimer m_timer;
    QDateTime m_simulationStart;
    int m_selectedRow = 0;
};

} // namespace qttutorial::plant_scada
