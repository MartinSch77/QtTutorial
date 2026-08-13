// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include "FleetIconDelegate.h"

#include <QDateTime>
#include <QHeaderView>
#include <QLabel>
#include <QSplitter>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

namespace qttutorial::two_wheelers::fleet {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_model(new FleetModel(10, this))
    , m_view(new QTableView(this))
    , m_trendWidget(new TrendWidget(this))
    , m_selectedVehicleLabel(new QLabel(tr("Select a vehicle to see its speed trend"), this))
    , m_utilizationTrendWidget(new TrendWidget(this))
    , m_utilizationLabel(new QLabel(tr("Fleet utilization"), this))
    , m_utilizationHistory(120)
    , m_historyStore(std::make_unique<TelemetryHistoryStore>(QStringLiteral(":memory:"),
                                                              QStringLiteral("two_wheelers_fleet_dashboard_history")))
{
    setWindowTitle(tr("QtTutorial - Two-Wheelers Fleet Dashboard"));

    m_view->setModel(m_model);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setItemDelegate(new FleetIconDelegate(m_view));
    m_view->verticalHeader()->setDefaultSectionSize(32);

    auto* splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(m_view);

    auto* trendContainer = new QWidget(this);
    auto* trendLayout = new QVBoxLayout(trendContainer);
    trendLayout->addWidget(m_selectedVehicleLabel);
    trendLayout->addWidget(m_trendWidget);
    splitter->addWidget(trendContainer);

    auto* utilizationContainer = new QWidget(this);
    auto* utilizationLayout = new QVBoxLayout(utilizationContainer);
    utilizationLayout->addWidget(m_utilizationLabel);
    utilizationLayout->addWidget(m_utilizationTrendWidget);
    splitter->addWidget(utilizationContainer);

    setCentralWidget(splitter);
    resize(960, 760);

    connect(m_model, &FleetModel::samplesUpdated, this, &MainWindow::onSamplesUpdated);
    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MainWindow::onSelectionChanged);
}

void MainWindow::onSamplesUpdated()
{
    refreshFleetUtilizationTrend();

    if (!m_historyStore->isOpen()) {
        return;
    }
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    for (int row = 0; row < m_model->rowCount(); ++row) {
        const VehicleSample sample = m_model->sampleAt(row);
        m_historyStore->recordSample(sample.id, now, sample.speedKph, sample.batteryPercent);
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
    m_trendWidget->setValues(speeds, 45.0);
}

void MainWindow::refreshFleetUtilizationTrend()
{
    std::vector<VehicleSample> samples;
    samples.reserve(static_cast<std::size_t>(m_model->rowCount()));
    for (int row = 0; row < m_model->rowCount(); ++row) {
        samples.push_back(m_model->sampleAt(row));
    }

    const double utilization = utilizationPercent(samples);
    m_utilizationHistory.addSample(utilization);
    m_utilizationLabel->setText(tr("Fleet utilization: %1% riding right now").arg(utilization, 0, 'f', 0));
    m_utilizationTrendWidget->setValues(m_utilizationHistory.values(), 100.0);
}

} // namespace qttutorial::two_wheelers::fleet
