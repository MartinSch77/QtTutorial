// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QDateTime>
#include <QHeaderView>
#include <QLabel>
#include <QSplitter>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

namespace qttutorial::agriculture::ops {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_model(new FieldOperationsModel(6, this))
    , m_view(new QTableView(this))
    , m_mapWidget(new FieldMapWidget(this))
    , m_selectedFieldLabel(new QLabel(tr("Select a field to see its position"), this))
    , m_historyStore(std::make_unique<OperationHistoryStore>(QStringLiteral(":memory:"),
                                                              QStringLiteral("farm_operations_history")))
{
    setWindowTitle(tr("QtTutorial - Farm Operations Dashboard"));

    m_view->setModel(m_model);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);

    auto* splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(m_view);

    auto* mapContainer = new QWidget(this);
    auto* mapLayout = new QVBoxLayout(mapContainer);
    mapLayout->addWidget(m_selectedFieldLabel);
    mapLayout->addWidget(m_mapWidget);
    splitter->addWidget(mapContainer);

    setCentralWidget(splitter);
    resize(900, 640);

    connect(m_model, &FieldOperationsModel::samplesUpdated, this, &MainWindow::onSamplesUpdated);
    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MainWindow::onSelectionChanged);
}

void MainWindow::onSamplesUpdated()
{
    if (!m_historyStore->isOpen()) {
        return;
    }
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    for (int row = 0; row < m_model->rowCount(); ++row) {
        const FieldSample sample = m_model->sampleAt(row);
        m_historyStore->recordSample(sample.fieldId, now, sample.coveragePercent, sample.status);
    }
    refreshMap();
}

void MainWindow::onSelectionChanged()
{
    refreshMap();
}

void MainWindow::refreshMap()
{
    const auto selected = m_view->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        m_selectedFieldLabel->setText(tr("Select a field to see its position"));
        m_mapWidget->setProgress(0.0, false);
        return;
    }

    const FieldSample sample = m_model->sampleAt(selected.first().row());
    m_selectedFieldLabel->setText(
        tr("%1 (%2) - pass %3, %4").arg(sample.fieldId, sample.vehicleId).arg(sample.passNumber).arg(sample.status));
    m_mapWidget->setProgress(sample.coveragePercent, true);
}

} // namespace qttutorial::agriculture::ops
