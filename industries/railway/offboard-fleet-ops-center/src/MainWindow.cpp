// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QSplitter>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

namespace qttutorial::fleet_ops {

namespace {
constexpr int kTickIntervalMs = 1000;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_history(QStringLiteral("fleet_ops_history"), QStringLiteral(":memory:"))
    , m_map(new FleetMapWidget(this))
    , m_model(new FleetTableModel(this))
    , m_table(new QTableView(this))
    , m_trend(new DelayTrendWidget(this))
    , m_networkTrend(new DelayTrendWidget(this))
    , m_trainSelector(new QComboBox(this))
    , m_simulationStart(QDateTime::currentDateTime())
{
    setWindowTitle(tr("Rail Fleet Operations Centre"));
    m_history.createSchema();

    for (int i = 0; i < m_config.trainCount; ++i) {
        m_trainSelector->addItem(QStringLiteral("T-%1").arg(i + 1, 2, 10, QLatin1Char('0')));
    }

    m_table->setModel(m_model);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);

    auto* central = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(central);

    auto* topRow = new QHBoxLayout();
    topRow->addWidget(new QLabel(tr("Delay trend for:"), this));
    topRow->addWidget(m_trainSelector);
    topRow->addStretch();
    mainLayout->addLayout(topRow);

    auto* mapAndTable = new QSplitter(Qt::Horizontal, this);
    mapAndTable->addWidget(m_map);
    mapAndTable->addWidget(m_table);
    mapAndTable->setStretchFactor(0, 1);
    mapAndTable->setStretchFactor(1, 2);

    auto* trends = new QSplitter(Qt::Horizontal, this);
    auto* trainTrendPane = new QWidget(this);
    auto* trainTrendLayout = new QVBoxLayout(trainTrendPane);
    trainTrendLayout->setContentsMargins(0, 0, 0, 0);
    trainTrendLayout->addWidget(new QLabel(tr("Selected train"), this));
    trainTrendLayout->addWidget(m_trend);
    auto* networkTrendPane = new QWidget(this);
    auto* networkTrendLayout = new QVBoxLayout(networkTrendPane);
    networkTrendLayout->setContentsMargins(0, 0, 0, 0);
    networkTrendLayout->addWidget(new QLabel(tr("Network-wide punctuality"), this));
    networkTrendLayout->addWidget(m_networkTrend);
    trends->addWidget(trainTrendPane);
    trends->addWidget(networkTrendPane);

    auto* splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(mapAndTable);
    splitter->addWidget(trends);
    mainLayout->addWidget(splitter);
    setCentralWidget(central);

    connect(m_trainSelector, &QComboBox::currentIndexChanged, this, &MainWindow::onTrainSelectionChanged);
    connect(&m_timer, &QTimer::timeout, this, &MainWindow::onTick);
    m_timer.start(kTickIntervalMs);

    resize(1180, 820);
}

void MainWindow::onTick()
{
    const double tSeconds = static_cast<double>(m_simulationStart.msecsTo(QDateTime::currentDateTime())) / 1000.0;
    const QDateTime now = QDateTime::currentDateTime();

    std::vector<TrainState> states = fleetStateAt(m_config, tSeconds);
    m_model->setStates(states);
    m_map->setFleet(m_config, states);

    for (const TrainState& state : states) {
        RunSample sample;
        sample.timestamp = now;
        sample.positionKm = state.positionKm;
        sample.speedKmh = state.speedKmh;
        sample.delayMinutes = state.delayMinutes;
        m_history.insertSample(state.trainId, sample);
    }

    refreshTrend();
    refreshNetworkTrend();
}

void MainWindow::onTrainSelectionChanged(int row)
{
    m_selectedTrainIndex = row;
    refreshTrend();
}

void MainWindow::refreshTrend()
{
    if (m_selectedTrainIndex < 0 || m_selectedTrainIndex >= m_config.trainCount) {
        return;
    }
    const QString trainId = QStringLiteral("T-%1").arg(m_selectedTrainIndex + 1, 2, 10, QLatin1Char('0'));
    const QDateTime to = QDateTime::currentDateTime();
    const QDateTime from = to.addSecs(-1800);
    m_trend->setSamples(m_history.samplesInRange(trainId, from, to));
}

void MainWindow::refreshNetworkTrend()
{
    constexpr qint64 kBucketSeconds = 30;
    const QDateTime to = QDateTime::currentDateTime();
    const QDateTime from = to.addSecs(-1800);

    const auto series = m_history.networkDelaySeries(from, to, kBucketSeconds);
    std::vector<RunSample> samples;
    samples.reserve(series.size());
    for (const auto& [bucketStart, averageDelayMinutes] : series) {
        RunSample sample;
        sample.timestamp = bucketStart;
        sample.delayMinutes = averageDelayMinutes;
        samples.push_back(sample);
    }
    m_networkTrend->setSamples(samples);
}

} // namespace qttutorial::fleet_ops
