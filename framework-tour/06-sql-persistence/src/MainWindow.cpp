// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QDateTime>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlTableModel>
#include <QTableView>
#include <QToolBar>

namespace qttutorial::sql_persistence {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_repository(std::make_unique<TaskRepository>(QStringLiteral("sql_persistence_app")))
    , m_model(nullptr)
    , m_view(new QTableView(this))
{
    setWindowTitle(tr("Qt SQL Persistence – Tasks"));

    if (!m_repository->applyMigrations()) {
        QMessageBox::critical(this, tr("Migration failed"), tr("Could not initialize the database schema."));
    }

    m_model = new QSqlTableModel(this, m_repository->database());
    m_model->setTable(QStringLiteral("tasks"));
    m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_model->select();
    m_model->setHeaderData(1, Qt::Horizontal, tr("Title"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Done"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Created"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Priority"));

    m_view->setModel(m_model);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->setColumnHidden(0, true);
    setCentralWidget(m_view);

    auto* toolBar = addToolBar(tr("Actions"));
    auto* addButton = new QPushButton(tr("Add task"), toolBar);
    auto* removeButton = new QPushButton(tr("Remove selected"), toolBar);
    auto* toggleButton = new QPushButton(tr("Toggle done"), toolBar);
    auto* saveButton = new QPushButton(tr("Save changes"), toolBar);
    toolBar->addWidget(addButton);
    toolBar->addWidget(removeButton);
    toolBar->addWidget(toggleButton);
    toolBar->addWidget(saveButton);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(removeButton, &QPushButton::clicked, this, &MainWindow::removeSelectedTask);
    connect(toggleButton, &QPushButton::clicked, this, &MainWindow::toggleSelectedDone);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveChanges);

    resize(640, 360);
}

void MainWindow::addTask()
{
    if (m_repository->addTask(tr("New task"), 0)) {
        m_model->select();
    }
}

void MainWindow::removeSelectedTask()
{
    const auto selection = m_view->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        return;
    }
    const int id = m_model->data(m_model->index(selection.first().row(), 0)).toInt();
    if (m_repository->removeTask(id)) {
        m_model->select();
    }
}

void MainWindow::toggleSelectedDone()
{
    const auto selection = m_view->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        return;
    }
    const int row = selection.first().row();
    const int id = m_model->data(m_model->index(row, 0)).toInt();
    const bool currentlyDone = m_model->data(m_model->index(row, 2)).toBool();
    if (m_repository->setDone(id, !currentlyDone)) {
        m_model->select();
    }
}

void MainWindow::saveChanges()
{
    if (!m_model->submitAll()) {
        QMessageBox::warning(this, tr("Save failed"), m_model->lastError().text());
    }
}

} // namespace qttutorial::sql_persistence
