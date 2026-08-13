// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include "BatteryTrendWidget.h"
#include "PassScheduleWidget.h"
#include "SatelliteTableModel.h"
#include "WorldMapWidget.h"

#include <QDateTime>
#include <QDir>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QStandardPaths>
#include <QTableView>

#include <utility>

namespace qttutorial::space {

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , m_fleetModel(new SatelliteTableModel(this))
    , m_fleetView(new QTableView(this))
    , m_trendWidget(new BatteryTrendWidget(this))
    , m_worldMap(new WorldMapWidget(m_stationTracker.stations(), this))
    , m_passSchedule(new PassScheduleWidget(this))
    , m_selectedLabel(new QLabel(this))
    , m_timer(new QTimer(this))
{
    setWindowTitle(tr("Mission Control"));

    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    m_history = std::make_unique<TelemetryHistoryStore>(QStringLiteral("mission_control_history"));
    const bool opened = m_history->open(dataDir.isEmpty() ? QStringLiteral("mission_control.sqlite")
                                                            : dataDir + QStringLiteral("/mission_control.sqlite"));
    Q_ASSERT(opened);

    m_fleetView->setModel(m_fleetModel);
    m_fleetView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_fleetView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_fleetView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_selectedLabel->setText(tr("Select a satellite to view its battery trend."));

    auto* layout = new QGridLayout(this);
    layout->addWidget(new QLabel(tr("Fleet"), this), 0, 0);
    layout->addWidget(m_fleetView, 1, 0);
    layout->addWidget(m_selectedLabel, 0, 1);
    layout->addWidget(m_trendWidget, 1, 1);
    layout->addWidget(new QLabel(tr("Fleet Ground Track"), this), 2, 0);
    layout->addWidget(m_worldMap, 3, 0);
    layout->addWidget(m_passSchedule, 3, 1);
    layout->setColumnStretch(0, 3);
    layout->setColumnStretch(1, 2);
    layout->setRowStretch(1, 2);
    layout->setRowStretch(3, 3);

    connect(m_fleetView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MainWindow::onSelectionChanged);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTick);
    m_timer->start(500);
    onTick();

    resize(1180, 820);
}

void MainWindow::onTick()
{
    m_simulator.step(30.0); // compress a multi-minute pass into a watchable demo

    const QDateTime now = QDateTime::currentDateTimeUtc();
    for (const SatelliteState& satellite : m_simulator.satellites()) {
        TelemetryRecord record;
        record.satelliteName = satellite.name;
        record.batteryPercent = satellite.batteryPercent;
        record.phaseDeg = satellite.phaseDeg;
        record.inEclipse = satellite.inEclipse;
        record.health = toString(satellite.health);
        record.timestamp = now;
        static_cast<void>(m_history->recordTelemetry(record));
    }

    m_fleetModel->setSatellites(m_simulator.satellites());
    m_worldMap->setFleet(m_simulator.satellites());

    std::vector<ContactWindow> windows;
    windows.reserve(m_simulator.satellites().size());
    for (const SatelliteState& satellite : m_simulator.satellites()) {
        if (auto window = m_stationTracker.nextContact(satellite, satellite.orbitalPeriodMinutes)) {
            windows.push_back(*window);
        }
    }
    m_passSchedule->setContactWindows(std::move(windows));

    onSelectionChanged();
}

void MainWindow::onSelectionChanged()
{
    const QModelIndexList selected = m_fleetView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return;
    }
    const SatelliteState* satellite = m_fleetModel->satelliteAt(selected.first().row());
    if (!satellite) {
        return;
    }
    m_selectedLabel->setText(tr("Battery trend - %1").arg(satellite->name));

    const auto records = m_history->history(satellite->name, 80);
    std::vector<double> batteryPercent;
    std::vector<bool> inEclipse;
    batteryPercent.reserve(records.size());
    inEclipse.reserve(records.size());
    for (auto it = records.rbegin(); it != records.rend(); ++it) {
        batteryPercent.push_back(it->batteryPercent);
        inEclipse.push_back(it->inEclipse);
    }
    m_trendWidget->setSeries(std::move(batteryPercent), std::move(inEclipse));
}

} // namespace qttutorial::space
