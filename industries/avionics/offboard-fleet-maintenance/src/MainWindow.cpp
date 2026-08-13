// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include "FleetTableModel.h"
#include "TrendWidget.h"

#include <QDateTime>
#include <QDir>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QStandardPaths>
#include <QTableView>

namespace qttutorial::avionics {

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , m_fleetModel(new FleetTableModel(this))
    , m_fleetView(new QTableView(this))
    , m_taskList(new QListWidget(this))
    , m_trendWidget(new TrendWidget(this))
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

    m_selectedLabel->setText(tr("Select an aircraft to view its engine vibration trend."));

    auto* layout = new QGridLayout(this);
    layout->addWidget(new QLabel(tr("Fleet"), this), 0, 0);
    layout->addWidget(m_fleetView, 1, 0, 1, 2);
    layout->addWidget(new QLabel(tr("Maintenance Tasks"), this), 2, 0);
    layout->addWidget(m_taskList, 3, 0);
    layout->addWidget(m_selectedLabel, 2, 1);
    layout->addWidget(m_trendWidget, 3, 1);
    layout->setRowStretch(1, 2);
    layout->setRowStretch(3, 3);

    connect(m_fleetView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MainWindow::onSelectionChanged);
    connect(&m_simulator, &FleetSimulator::maintenanceTaskCreated, this, &MainWindow::onMaintenanceTaskCreated);

    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTick);
    m_timer->start(500);
    onTick();

    resize(900, 560);
}

void MainWindow::onTick()
{
    m_simulator.advance(0.5);
    m_fleetModel->setAircraft(m_simulator.aircraft());

    const QDateTime now = QDateTime::currentDateTimeUtc();
    for (const Aircraft& aircraft : m_simulator.aircraft()) {
        ParameterSample sample;
        sample.aircraftTail = aircraft.tailNumber;
        sample.parameterName = QStringLiteral("engine1_vibration_ips");
        sample.value = aircraft.engine1.vibrationIps;
        sample.timestamp = now;
        static_cast<void>(m_history->recordSample(sample));
    }

    onSelectionChanged();
}

void MainWindow::onSelectionChanged()
{
    const QModelIndexList selected = m_fleetView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return;
    }
    const Aircraft* aircraft = m_fleetModel->aircraftAt(selected.first().row());
    if (!aircraft) {
        return;
    }
    m_selectedLabel->setText(tr("Engine 1 vibration trend for %1").arg(aircraft->tailNumber));

    const auto samples = m_history->history(aircraft->tailNumber, QStringLiteral("engine1_vibration_ips"), 60);
    std::vector<double> values;
    values.reserve(samples.size());
    for (auto it = samples.rbegin(); it != samples.rend(); ++it) {
        values.push_back(it->value);
    }
    m_trendWidget->setSeries(std::move(values), 0.0, 0.45);
}

void MainWindow::onMaintenanceTaskCreated(const MaintenanceTask& task)
{
    m_taskList->addItem(QStringLiteral("[%1] %2 - %3").arg(task.priority, task.aircraftTail, task.description));
}

} // namespace qttutorial::avionics
