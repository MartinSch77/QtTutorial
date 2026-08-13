// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <QComboBox>
#include <QDateTime>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QSplitter>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

namespace qttutorial::plant_scada {

namespace {
constexpr int kTickIntervalMs = 1000;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_historian(QStringLiteral("plant_scada_historian"), QStringLiteral(":memory:"))
    , m_tagModel(new TagTableModel(defaultTags(), this))
    , m_overviewModel(new PlantOverviewModel(this))
    , m_tagTable(new QTableView(this))
    , m_overview(new PlantOverviewWidget(this))
    , m_trend(new TrendWidget(this))
    , m_alarmList(new QListWidget(this))
    , m_tagSelector(new QComboBox(this))
    , m_simulationStart(QDateTime::currentDateTime())
    , m_latestValues(defaultTags().size(), 0.0)
{
    setWindowTitle(tr("Plant SCADA Historian"));
    m_historian.createSchema();

    for (const TagDefinition& tag : defaultTags()) {
        m_tagSelector->addItem(tag.line + QStringLiteral(" — ") + tag.tagId);
    }

    m_tagTable->setModel(m_tagModel);
    m_tagTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tagTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_overview->setModel(m_overviewModel);

    auto* central = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(central);

    mainLayout->addWidget(new QLabel(tr("Plant overview"), this));
    mainLayout->addWidget(m_overview);

    auto* topRow = new QHBoxLayout();
    topRow->addWidget(new QLabel(tr("Trend for:"), this));
    topRow->addWidget(m_tagSelector);
    topRow->addStretch();
    mainLayout->addLayout(topRow);

    auto* splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(m_tagTable);

    auto* lowerSplitter = new QSplitter(Qt::Horizontal, this);
    lowerSplitter->addWidget(m_trend);
    lowerSplitter->addWidget(m_alarmList);
    lowerSplitter->setStretchFactor(0, 3);
    lowerSplitter->setStretchFactor(1, 1);

    splitter->addWidget(lowerSplitter);
    mainLayout->addWidget(splitter);
    setCentralWidget(central);

    connect(m_tagSelector, &QComboBox::currentIndexChanged, this, &MainWindow::onTagSelectionChanged);
    connect(&m_timer, &QTimer::timeout, this, &MainWindow::onTick);
    m_timer.start(kTickIntervalMs);

    resize(960, 760);
}

void MainWindow::onTick()
{
    const double tSeconds = static_cast<double>(m_simulationStart.msecsTo(QDateTime::currentDateTime())) / 1000.0;
    const QDateTime now = QDateTime::currentDateTime();

    const auto& tags = defaultTags();
    for (int row = 0; row < static_cast<int>(tags.size()); ++row) {
        const TagDefinition& tag = tags[static_cast<std::size_t>(row)];
        const double value = valueAt(tag, tSeconds);
        m_tagModel->updateValue(row, value);
        m_latestValues[static_cast<std::size_t>(row)] = value;
        m_historian.insertSample(tag.tagId, now, value);

        const Severity severity = evaluate(tag, value);
        if (severity != Severity::Normal) {
            m_alarmList->insertItem(0, QStringLiteral("[%1] %2 %3 (%4)")
                                             .arg(now.toString(Qt::ISODateWithMs), tag.tagId,
                                                  QString::number(value, 'f', 2), severityLabel(severity)));
            while (m_alarmList->count() > 100) {
                delete m_alarmList->takeItem(m_alarmList->count() - 1);
            }
        }
    }

    m_overviewModel->update(tags, m_latestValues);
    refreshTrend();
}

void MainWindow::onTagSelectionChanged(int row)
{
    m_selectedRow = row;
    refreshTrend();
}

void MainWindow::refreshTrend()
{
    if (m_selectedRow < 0 || m_selectedRow >= static_cast<int>(defaultTags().size())) {
        return;
    }
    const TagDefinition& tag = defaultTags()[static_cast<std::size_t>(m_selectedRow)];
    const QDateTime to = QDateTime::currentDateTime();
    const QDateTime from = to.addSecs(-600);
    const std::vector<Sample> samples = m_historian.samplesInRange(tag.tagId, from, to);
    m_trend->setSamples(samples, tag.criticalLow, tag.criticalHigh);
}

} // namespace qttutorial::plant_scada
