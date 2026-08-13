// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QDateTime>
#include <QHeaderView>
#include <QLabel>
#include <QSplitter>
#include <QTableView>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

#include <vector>

namespace qttutorial::agriculture::ops {

namespace {
constexpr int kTrendHistoryLimit = 60;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_model(new FieldOperationsModel(6, this))
    , m_view(new QTableView(this))
    , m_overviewWidget(new FieldStatusOverviewWidget(this))
    , m_detailTabs(new QTabWidget(this))
    , m_mapWidget(new FieldMapWidget(this))
    , m_fuelTrendWidget(new TrendChartWidget(this))
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

    auto* upperContainer = new QWidget(this);
    auto* upperLayout = new QVBoxLayout(upperContainer);
    upperLayout->addWidget(m_overviewWidget);
    upperLayout->addWidget(m_view);
    splitter->addWidget(upperContainer);

    auto* mapContainer = new QWidget(this);
    auto* mapLayout = new QVBoxLayout(mapContainer);
    mapLayout->addWidget(m_mapWidget);

    m_detailTabs->addTab(mapContainer, tr("Field Map"));
    m_detailTabs->addTab(m_fuelTrendWidget, tr("Fuel Trend"));

    auto* lowerContainer = new QWidget(this);
    auto* lowerLayout = new QVBoxLayout(lowerContainer);
    lowerLayout->addWidget(m_selectedFieldLabel);
    lowerLayout->addWidget(m_detailTabs);
    splitter->addWidget(lowerContainer);

    setCentralWidget(splitter);
    resize(1000, 760);

    connect(m_model, &FieldOperationsModel::samplesUpdated, this, &MainWindow::onSamplesUpdated);
    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MainWindow::onSelectionChanged);
}

void MainWindow::onSamplesUpdated()
{
    std::vector<FieldSample> samples;
    samples.reserve(static_cast<std::size_t>(m_model->rowCount()));
    for (int row = 0; row < m_model->rowCount(); ++row) {
        samples.push_back(m_model->sampleAt(row));
    }
    m_overviewWidget->setSamples(samples);

    if (m_historyStore->isOpen()) {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        for (const FieldSample& sample : samples) {
            m_historyStore->recordSample(sample.fieldId, now, sample.coveragePercent, sample.status,
                                          sample.engineLoadPercent, sample.fuelLevelPercent);
        }
    }
    refreshMap();
    refreshTrend();
}

void MainWindow::onSelectionChanged()
{
    refreshMap();
    refreshTrend();
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
        tr("%1 (%2) - pass %3/%4, %5").arg(sample.fieldId, sample.vehicleId).arg(sample.passNumber)
            .arg(sample.plannedPasses).arg(sample.status));
    m_mapWidget->setProgress(sample.coveragePercent, true);
}

void MainWindow::refreshTrend()
{
    const auto selected = m_view->selectionModel()->selectedRows();
    if (selected.isEmpty() || !m_historyStore->isOpen()) {
        m_fuelTrendWidget->clear();
        return;
    }

    const FieldSample sample = m_model->sampleAt(selected.first().row());
    const std::vector<double> recentFuel = m_historyStore->recentFuelLevels(sample.fieldId, kTrendHistoryLimit);
    m_fuelTrendWidget->setSeries(recentFuel, 0.0, 100.0, QStringLiteral("%"));
}

} // namespace qttutorial::agriculture::ops
