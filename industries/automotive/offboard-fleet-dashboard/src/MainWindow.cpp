// SPDX-License-Identifier: MIT
#include "MainWindow.h"
#include "FleetMaintenanceDelegate.h"

#include <QDateTime>
#include <QHeaderView>
#include <QLabel>
#include <QSplitter>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

namespace qttutorial::automotive::fleet {

namespace {
constexpr std::size_t kFleetTrendHistoryLength = 60;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_model(new FleetModel(8, this))
    , m_view(new QTableView(this))
    , m_trendWidget(new TrendWidget(this))
    , m_fleetTrendWidget(new TrendWidget(this))
    , m_selectedVehicleLabel(new QLabel(tr("Select a vehicle to see its speed trend"), this))
    , m_fleetTrendLabel(new QLabel(tr("Fleet-wide efficiency trend"), this))
    , m_historyStore(std::make_unique<TelemetryHistoryStore>(QStringLiteral(":memory:"),
                                                              QStringLiteral("fleet_dashboard_history")))
{
    setWindowTitle(tr("QtTutorial - Fleet Dashboard"));

    m_view->setModel(m_model);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setItemDelegateForColumn(FleetModel::MaintenanceColumn, new FleetMaintenanceDelegate(m_view));

    auto* splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(m_view);

    auto* trendContainer = new QWidget(this);
    auto* trendLayout = new QVBoxLayout(trendContainer);
    trendLayout->addWidget(m_selectedVehicleLabel);
    trendLayout->addWidget(m_trendWidget);
    trendLayout->addWidget(m_fleetTrendLabel);
    trendLayout->addWidget(m_fleetTrendWidget);
    splitter->addWidget(trendContainer);

    setCentralWidget(splitter);
    resize(960, 720);

    connect(m_model, &FleetModel::samplesUpdated, this, &MainWindow::onSamplesUpdated);
    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MainWindow::onSelectionChanged);
}

void MainWindow::onSamplesUpdated()
{
    double efficiencySum = 0.0;
    const int rowCount = m_model->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        const VehicleSample sample = m_model->sampleAt(row);
        efficiencySum += sample.efficiencyPercent;
        if (m_historyStore->isOpen()) {
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            m_historyStore->recordSample(sample.id, now, sample.speedKph, sample.fuelPercent);
        }
    }

    if (rowCount > 0) {
        m_fleetEfficiencyHistory.push_back(efficiencySum / rowCount);
        while (m_fleetEfficiencyHistory.size() > kFleetTrendHistoryLength) {
            m_fleetEfficiencyHistory.pop_front();
        }
    }

    refreshTrend();
    refreshFleetTrend();
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

void MainWindow::refreshFleetTrend()
{
    const std::vector<double> history(m_fleetEfficiencyHistory.begin(), m_fleetEfficiencyHistory.end());
    m_fleetTrendWidget->setValues(history, 100.0);
}

} // namespace qttutorial::automotive::fleet
