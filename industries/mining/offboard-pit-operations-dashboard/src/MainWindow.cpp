// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QDateTime>
#include <QHeaderView>
#include <QLabel>
#include <QSplitter>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <cstddef>

namespace qttutorial::mining::pit {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_model(new PitFleetModel(6, this))
    , m_view(new QTableView(this))
    , m_trendWidget(new ProductionTrendWidget(this))
    , m_fleetTrendWidget(new ProductionTrendWidget(this))
    , m_pitMapWidget(new PitMapWidget(this))
    , m_selectedTruckLabel(new QLabel(tr("Select a truck to see its production trend"), this))
    , m_fleetTonnesLabel(new QLabel(tr("Fleet cumulative tonnes hauled"), this))
    , m_historyStore(std::make_unique<HaulHistoryStore>(QStringLiteral(":memory:"),
                                                         QStringLiteral("pit_operations_history")))
{
    setWindowTitle(tr("QtTutorial - Pit Operations Dashboard"));

    m_view->setModel(m_model);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);

    auto* leftSplitter = new QSplitter(Qt::Vertical, this);
    leftSplitter->addWidget(m_view);

    auto* trendContainer = new QWidget(this);
    auto* trendLayout = new QVBoxLayout(trendContainer);
    trendLayout->addWidget(m_selectedTruckLabel);
    trendLayout->addWidget(m_trendWidget);
    leftSplitter->addWidget(trendContainer);

    auto* pitMapContainer = new QWidget(this);
    auto* pitMapLayout = new QVBoxLayout(pitMapContainer);
    pitMapLayout->addWidget(new QLabel(tr("Pit overview - live truck positions"), this));
    pitMapLayout->addWidget(m_pitMapWidget);

    auto* fleetTrendContainer = new QWidget(this);
    auto* fleetTrendLayout = new QVBoxLayout(fleetTrendContainer);
    fleetTrendLayout->addWidget(m_fleetTonnesLabel);
    fleetTrendLayout->addWidget(m_fleetTrendWidget);

    auto* rightSplitter = new QSplitter(Qt::Vertical, this);
    rightSplitter->addWidget(pitMapContainer);
    rightSplitter->addWidget(fleetTrendContainer);

    auto* mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(leftSplitter);
    mainSplitter->addWidget(rightSplitter);

    setCentralWidget(mainSplitter);
    resize(1200, 700);

    connect(m_model, &PitFleetModel::samplesUpdated, this, &MainWindow::onSamplesUpdated);
    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MainWindow::onSelectionChanged);
}

void MainWindow::onSamplesUpdated()
{
    m_pitMapWidget->setSamples(m_model->samples());

    if (!m_historyStore->isOpen()) {
        return;
    }
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    double fleetTonnes = 0.0;
    for (int row = 0; row < m_model->rowCount(); ++row) {
        const TruckSample sample = m_model->sampleAt(row);
        m_historyStore->recordSample(sample.id, now, sample.stateLabel, sample.payloadTonnes,
                                      sample.cumulativeTonnesHauled);
        fleetTonnes += sample.cumulativeTonnesHauled;
    }

    constexpr std::size_t kMaxFleetHistoryPoints = 120;
    m_fleetTonnesHistory.push_back(fleetTonnes);
    if (m_fleetTonnesHistory.size() > kMaxFleetHistoryPoints) {
        m_fleetTonnesHistory.erase(m_fleetTonnesHistory.begin());
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
        m_selectedTruckLabel->setText(tr("Select a truck to see its production trend"));
        m_trendWidget->setValues({}, 1.0);
        return;
    }

    const TruckSample sample = m_model->sampleAt(selected.first().row());
    m_selectedTruckLabel->setText(tr("Cumulative tonnes hauled: %1 (%2)").arg(sample.id, sample.location));
    const std::vector<double> tonnes = m_historyStore->recentCumulativeTonnes(sample.id, 60);
    const double maxValue = std::max(1.0, sample.cumulativeTonnesHauled + HaulFleetSimulator::kRatedCapacityTonnes);
    m_trendWidget->setValues(tonnes, maxValue);
}

void MainWindow::refreshFleetTrend()
{
    if (m_fleetTonnesHistory.empty()) {
        m_fleetTrendWidget->setValues({}, 1.0);
        return;
    }
    const double latest = m_fleetTonnesHistory.back();
    m_fleetTonnesLabel->setText(
        tr("Fleet cumulative tonnes hauled: %1 t").arg(latest, 0, 'f', 0));
    const double maxValue = std::max(1.0, latest + HaulFleetSimulator::kRatedCapacityTonnes * m_model->rowCount());
    m_fleetTrendWidget->setValues(m_fleetTonnesHistory, maxValue);
}

} // namespace qttutorial::mining::pit
