// SPDX-License-Identifier: MIT
#pragma once

#include "TaskRepository.h"

#include <QMainWindow>

#include <memory>

class QSqlTableModel;
class QTableView;

namespace qttutorial::sql_persistence {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void addTask();
    void removeSelectedTask();
    void toggleSelectedDone();
    void saveChanges();

    std::unique_ptr<TaskRepository> m_repository;
    QSqlTableModel* m_model;
    QTableView* m_view;
};

} // namespace qttutorial::sql_persistence
