// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QDateTime>
#include <QHeaderView>
#include <QLabel>
#include <QSplitter>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

namespace qttutorial::automotive::fleet {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_model(new FleetModel(8, this))
    , m_view(new QTableView(this))
    , m_trendWidget(new TrendWidget(this))
    , m_selectedVehicleLabel(new QLabel(tr("Select a vehicle to see its speed trend"), this))
    , m_historyStore(std::make_unique<TelemetryHistoryStore>(QStringLiteral(":memory:"),
                                                              QStringLiteral("fleet_dashboard_history")))
{
    setWindowTitle(tr("QtTutorial - Fleet Dashboard"));

    m_view->setModel(m_model);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);

    auto* splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(m_view);

    auto* trendContainer = new QWidget(this);
    auto* trendLayout = new QVBoxLayout(trendContainer);
    trendLayout->addWidget(m_selectedVehicleLabel);
    trendLayout->addWidget(m_trendWidget);
    splitter->addWidget(trendContainer);

    setCentralWidget(splitter);
    resize(900, 600);

    connect(m_model, &FleetModel::samplesUpdated, this, &MainWindow::onSamplesUpdated);
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
        const VehicleSample sample = m_model->sampleAt(row);
        m_historyStore->recordSample(sample.id, now, sample.speedKph, sample.fuelPercent);
    }
    refreshTrend();
}

void MainWindow::onSelectionChanged()
{
    refreshTrend();
}

void MainWindow::refreshTrend()
{
    const auto selected = m_view->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        m_selectedVehicleLabel->setText(tr("Select a vehicle to see its speed trend"));
        m_trendWidget->setValues({}, 1.0);
        return;
    }

    const VehicleSample sample = m_model->sampleAt(selected.first().row());
    m_selectedVehicleLabel->setText(tr("Speed trend: %1 (%2)").arg(sample.id, sample.location));
    const std::vector<double> speeds = m_historyStore->recentSpeeds(sample.id, 60);
    m_trendWidget->setValues(speeds, 150.0);
}

} // namespace qttutorial::automotive::fleet
