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

namespace qttutorial::mining::pit {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_model(new PitFleetModel(6, this))
    , m_view(new QTableView(this))
    , m_trendWidget(new ProductionTrendWidget(this))
    , m_selectedTruckLabel(new QLabel(tr("Select a truck to see its production trend"), this))
    , m_historyStore(std::make_unique<HaulHistoryStore>(QStringLiteral(":memory:"),
                                                         QStringLiteral("pit_operations_history")))
{
    setWindowTitle(tr("QtTutorial - Pit Operations Dashboard"));

    m_view->setModel(m_model);
    m_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);

    auto* splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(m_view);

    auto* trendContainer = new QWidget(this);
    auto* trendLayout = new QVBoxLayout(trendContainer);
    trendLayout->addWidget(m_selectedTruckLabel);
    trendLayout->addWidget(m_trendWidget);
    splitter->addWidget(trendContainer);

    setCentralWidget(splitter);
    resize(900, 600);

    connect(m_model, &PitFleetModel::samplesUpdated, this, &MainWindow::onSamplesUpdated);
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
        const TruckSample sample = m_model->sampleAt(row);
        m_historyStore->recordSample(sample.id, now, sample.stateLabel, sample.payloadTonnes,
                                      sample.cumulativeTonnesHauled);
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

} // namespace qttutorial::mining::pit
