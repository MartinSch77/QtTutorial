// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include "FleetTableModel.h"
#include "MaintenanceOverviewWidget.h"
#include "TrendWidget.h"

#include <QComboBox>
#include <QDateTime>
#include <QDir>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QStandardPaths>
#include <QTableView>

#include <iterator>

namespace qttutorial::avionics {

namespace {
struct TrendParameter {
    const char* key;
    const char* label;
    double nominalLow;
    double nominalHigh;
};

constexpr TrendParameter kTrendParameters[] = {
    {"engine1_vibration_ips", "Eng1 Vibration (ips)", 0.0, 0.45},
    {"engine1_egt_margin_c", "Eng1 EGT Margin (C)", 20.0, 80.0},
    {"engine1_oil_pressure_psi", "Eng1 Oil Pressure (psi)", 45.0, 85.0},
};
} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , m_fleetModel(new FleetTableModel(this))
    , m_fleetView(new QTableView(this))
    , m_taskList(new QListWidget(this))
    , m_trendWidget(new TrendWidget(this))
    , m_trendParameterSelector(new QComboBox(this))
    , m_maintenanceOverview(new MaintenanceOverviewWidget(this))
    , m_selectedLabel(new QLabel(this))
    , m_timer(new QTimer(this))
{
    setWindowTitle(tr("Fleet Maintenance Dashboard"));

    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    m_history = std::make_unique<FlightHistoryStore>(QStringLiteral("fleet_maintenance_history"));
    const bool opened = m_history->open(dataDir.isEmpty() ? QStringLiteral("fleet_maintenance.sqlite")
                                                            : dataDir + QStringLiteral("/fleet_maintenance.sqlite"));
    Q_ASSERT(opened);

    m_fleetView->setModel(m_fleetModel);
    m_fleetView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_fleetView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_fleetView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_selectedLabel->setText(tr("Select an aircraft to view a trend."));

    for (const auto& parameter : kTrendParameters) {
        m_trendParameterSelector->addItem(QString::fromLatin1(parameter.label));
    }

    auto* layout = new QGridLayout(this);
    layout->addWidget(new QLabel(tr("Fleet"), this), 0, 0);
    layout->addWidget(m_fleetView, 1, 0, 1, 2);
    layout->addWidget(new QLabel(tr("Maintenance Tasks"), this), 2, 0);
    layout->addWidget(m_taskList, 3, 0);

    auto* trendHeaderLayout = new QGridLayout();
    trendHeaderLayout->addWidget(m_selectedLabel, 0, 0);
    trendHeaderLayout->addWidget(m_trendParameterSelector, 0, 1);
    layout->addLayout(trendHeaderLayout, 2, 1);
    layout->addWidget(m_trendWidget, 3, 1);

    layout->addWidget(new QLabel(tr("Fleet-Wide Maintenance Due"), this), 4, 0, 1, 2);
    layout->addWidget(m_maintenanceOverview, 5, 0, 1, 2);

    layout->setRowStretch(1, 2);
    layout->setRowStretch(3, 3);
    layout->setRowStretch(5, 2);

    connect(m_fleetView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MainWindow::onSelectionChanged);
    connect(m_trendParameterSelector, &QComboBox::currentIndexChanged, this, &MainWindow::updateTrend);
    connect(&m_simulator, &FleetSimulator::maintenanceTaskCreated, this, &MainWindow::onMaintenanceTaskCreated);

    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTick);
    m_timer->start(500);
    onTick();

    resize(960, 760);
}

void MainWindow::onTick()
{
    m_simulator.advance(0.5);
    m_fleetModel->setAircraft(m_simulator.aircraft());

    const QDateTime now = QDateTime::currentDateTimeUtc();
    const auto& fleet = m_simulator.aircraft();
    for (const Aircraft& aircraft : fleet) {
        const struct { const char* name; double value; } samples[] = {
            {"engine1_vibration_ips", aircraft.engine1.vibrationIps},
            {"engine1_egt_margin_c", aircraft.engine1.egtMarginC},
            {"engine1_oil_pressure_psi", aircraft.engine1.oilPressurePsi},
        };
        for (const auto& entry : samples) {
            ParameterSample sample;
            sample.aircraftTail = aircraft.tailNumber;
            sample.parameterName = QString::fromLatin1(entry.name);
            sample.value = entry.value;
            sample.timestamp = now;
            static_cast<void>(m_history->recordSample(sample));
        }
    }

    std::vector<FleetOverviewEntry> overviewEntries;
    overviewEntries.reserve(fleet.size());
    for (std::size_t i = 0; i < fleet.size(); ++i) {
        overviewEntries.push_back({fleet[i].tailNumber, m_simulator.inspectionStatus(i)});
    }
    m_maintenanceOverview->setEntries(std::move(overviewEntries));

    updateTrend();
}

void MainWindow::onSelectionChanged()
{
    updateTrend();
}

void MainWindow::updateTrend()
{
    const QModelIndexList selected = m_fleetView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return;
    }
    const Aircraft* aircraft = m_fleetModel->aircraftAt(selected.first().row());
    if (!aircraft) {
        return;
    }

    const int parameterIndex = m_trendParameterSelector->currentIndex();
    if (parameterIndex < 0 || parameterIndex >= static_cast<int>(std::size(kTrendParameters))) {
        return;
    }
    const TrendParameter& parameter = kTrendParameters[parameterIndex];
    m_selectedLabel->setText(tr("%1 for %2").arg(QString::fromLatin1(parameter.label), aircraft->tailNumber));

    const auto samples = m_history->history(aircraft->tailNumber, QString::fromLatin1(parameter.key), 60);
    std::vector<double> values;
    values.reserve(samples.size());
    for (auto it = samples.rbegin(); it != samples.rend(); ++it) {
        values.push_back(it->value);
    }
    m_trendWidget->setSeries(std::move(values), parameter.nominalLow, parameter.nominalHigh);
}

void MainWindow::onMaintenanceTaskCreated(const MaintenanceTask& task)
{
    m_taskList->addItem(QStringLiteral("[%1] %2 - %3").arg(task.priority, task.aircraftTail, task.description));
}

} // namespace qttutorial::avionics
