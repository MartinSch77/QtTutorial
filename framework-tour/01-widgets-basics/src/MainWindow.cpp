// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QHeaderView>
#include <QPushButton>
#include <QTableView>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

namespace qttutorial::widgets_basics {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_model(new UnitConverterModel(this))
    , m_view(new QTableView(this))
{
    setWindowTitle(tr("Qt Widgets Basics – Unit Converter"));

    m_view->setModel(m_model);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setCentralWidget(m_view);

    auto* toolBar = addToolBar(tr("Actions"));
    auto* addButton = new QPushButton(tr("Add row"), toolBar);
    auto* removeButton = new QPushButton(tr("Remove selected"), toolBar);
    toolBar->addWidget(addButton);
    toolBar->addWidget(removeButton);

    connect(addButton, &QPushButton::clicked, this, [this] {
        m_model->addRow({1.0, QStringLiteral("m"), QStringLiteral("ft")});
    });
    connect(removeButton, &QPushButton::clicked, this, [this] {
        const auto selection = m_view->selectionModel()->selectedRows();
        if (!selection.isEmpty()) {
            m_model->removeRow(selection.first().row());
        }
    });

    resize(560, 320);
}

} // namespace qttutorial::widgets_basics
