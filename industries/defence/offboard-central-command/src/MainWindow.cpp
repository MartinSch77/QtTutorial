// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include "AssetTableModel.h"
#include "FleetReadiness.h"
#include "FleetReadinessBoard.h"
#include "TacticalMapWidget.h"

#include <QDateTime>
#include <QDir>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QStandardPaths>
#include <QVBoxLayout>

namespace qttutorial::defence {

namespace {
QColor colorForSeverity(AlertSeverity severity)
{
    switch (severity) {
    case AlertSeverity::Critical:
        return QColor(0xc0, 0x39, 0x2b);
    case AlertSeverity::Caution:
        return QColor(0xb8, 0x86, 0x0b);
    case AlertSeverity::Info:
        return QColor(0x39, 0xc0, 0xff);
    }
    return QColor(Qt::white);
}
}

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , m_assetModel(new AssetTableModel(this))
    , m_assetView(new QTableView(this))
    , m_mapWidget(new TacticalMapWidget(this))
    , m_readinessBoard(new FleetReadinessBoard(this))
    , m_severityFilter(new QComboBox(this))
    , m_alertList(new QListWidget(this))
    , m_timer(new QTimer(this))
{
    setWindowTitle(tr("Central Command"));

    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    m_history = std::make_unique<AssetHistoryStore>(QStringLiteral("central_command_history"));
    const bool opened = m_history->open(dataDir.isEmpty() ? QStringLiteral("central_command.sqlite")
                                                            : dataDir + QStringLiteral("/central_command.sqlite"));
    Q_ASSERT(opened);

    m_assetView->setModel(m_assetModel);
    m_assetView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_assetView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_assetView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_severityFilter->addItem(tr("All severities"), QVariant::fromValue(static_cast<int>(AlertSeverity::Info)));
    m_severityFilter->addItem(tr("Caution and above"), QVariant::fromValue(static_cast<int>(AlertSeverity::Caution)));
    m_severityFilter->addItem(tr("Critical only"), QVariant::fromValue(static_cast<int>(AlertSeverity::Critical)));
    connect(m_severityFilter, &QComboBox::currentIndexChanged, this, &MainWindow::refreshAlertList);

    auto* alertHeaderRow = new QWidget(this);
    auto* alertHeaderLayout = new QHBoxLayout(alertHeaderRow);
    alertHeaderLayout->setContentsMargins(0, 0, 0, 0);
    alertHeaderLayout->addWidget(new QLabel(tr("Event / Alert Log"), alertHeaderRow));
    alertHeaderLayout->addStretch();
    alertHeaderLayout->addWidget(new QLabel(tr("Filter:"), alertHeaderRow));
    alertHeaderLayout->addWidget(m_severityFilter);

    auto* layout = new QGridLayout(this);
    layout->addWidget(new QLabel(tr("Tactical Map"), this), 0, 0);
    layout->addWidget(m_mapWidget, 1, 0, 2, 1);
    layout->addWidget(new QLabel(tr("Fleet Readiness"), this), 3, 0);
    layout->addWidget(m_readinessBoard, 4, 0);
    layout->addWidget(new QLabel(tr("Assets"), this), 0, 1);
    layout->addWidget(m_assetView, 1, 1);
    layout->addWidget(alertHeaderRow, 2, 1);
    layout->addWidget(m_alertList, 3, 1, 2, 1);
    layout->setColumnStretch(0, 3);
    layout->setColumnStretch(1, 3);
    layout->setRowStretch(1, 2);
    layout->setRowStretch(3, 2);

    connect(&m_simulator, &FleetSimulator::alertRaised, this, &MainWindow::onAlertRaised);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::onTick);
    m_timer->start(500);
    onTick();

    resize(1000, 700);
}

void MainWindow::onTick()
{
    m_simulator.advance(0.5);

    const QDateTime now = QDateTime::currentDateTimeUtc();
    for (const Asset& asset : m_simulator.assets()) {
        AssetHistoryRecord record;
        record.assetId = asset.id;
        record.type = asset.type;
        record.xKm = asset.xKm;
        record.yKm = asset.yKm;
        record.headingDeg = asset.headingDeg;
        record.health = asset.health;
        record.timestamp = now;
        static_cast<void>(m_history->recordStatus(record));
    }

    m_assetModel->setAssets(m_simulator.assets());
    m_mapWidget->setAssets(m_simulator.assets());
    m_readinessBoard->setSummary(summarizeReadiness(m_simulator.assets()));
}

void MainWindow::onAlertRaised(const Alert& alert)
{
    m_alertLog.addAlert(alert);
    refreshAlertList();
}

AlertSeverity MainWindow::selectedMinimumSeverity() const
{
    return static_cast<AlertSeverity>(m_severityFilter->currentData().toInt());
}

void MainWindow::refreshAlertList()
{
    m_alertList->clear();
    for (const Alert& alert : m_alertLog.alertsBySeverity(selectedMinimumSeverity())) {
        auto* item = new QListWidgetItem(
            QStringLiteral("[%1] %2 - %3").arg(alert.timestamp.toString(Qt::ISODate), alert.assetId, alert.message));
        item->setForeground(colorForSeverity(alert.severity));
        m_alertList->addItem(item);
    }
}

} // namespace qttutorial::defence
