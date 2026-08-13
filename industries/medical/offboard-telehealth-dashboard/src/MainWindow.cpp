// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include "PatientOverviewWidget.h"
#include "SparklineDelegate.h"
#include "TrendChartWidget.h"

#include <QColor>
#include <QDateTime>
#include <QLabel>
#include <QListView>
#include <QListWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>

namespace qttutorial::medical::telehealth {

namespace {
constexpr int kPrimaryPatientRow = 0;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_model(new PatientListModel(5, this))
    , m_overview(new PatientOverviewWidget(m_model, this))
    , m_patientList(new QListView(this))
    , m_alertList(new QListWidget(this))
    , m_trendChart(new TrendChartWidget(this))
    , m_historyStore(std::make_unique<VitalsHistoryStore>(QStringLiteral(":memory:"),
                                                           QStringLiteral("telehealth_dashboard_history")))
{
    setWindowTitle(tr("QtTutorial - Telehealth Dashboard"));

    m_patientList->setModel(m_model);
    m_patientList->setItemDelegate(new SparklineDelegate(this));
    m_patientList->setUniformItemSizes(false);

    auto* outerSplitter = new QSplitter(Qt::Vertical, this);

    auto* overviewContainer = new QWidget(this);
    auto* overviewLayout = new QVBoxLayout(overviewContainer);
    overviewLayout->addWidget(new QLabel(tr("Patient overview (click a card to trend it below)"), this));
    overviewLayout->addWidget(m_overview);
    outerSplitter->addWidget(overviewContainer);

    auto* middleSplitter = new QSplitter(Qt::Horizontal, this);
    auto* patientContainer = new QWidget(this);
    auto* patientLayout = new QVBoxLayout(patientContainer);
    patientLayout->addWidget(new QLabel(tr("Patients"), this));
    patientLayout->addWidget(m_patientList);
    middleSplitter->addWidget(patientContainer);

    auto* alertContainer = new QWidget(this);
    auto* alertLayout = new QVBoxLayout(alertContainer);
    alertLayout->addWidget(new QLabel(tr("Alerts (most severe first)"), this));
    alertLayout->addWidget(m_alertList);
    middleSplitter->addWidget(alertContainer);
    outerSplitter->addWidget(middleSplitter);

    outerSplitter->addWidget(m_trendChart);

    setCentralWidget(outerSplitter);
    resize(880, 900);

    connect(m_model, &PatientListModel::samplesUpdated, this, &MainWindow::onSamplesUpdated);
    connect(m_overview, &PatientOverviewWidget::patientSelected, this, &MainWindow::onPatientSelected);

    m_trendChart->setHistoryStore(m_historyStore.get());
    const PatientVitals primary = m_model->vitalsAt(kPrimaryPatientRow);
    onPatientSelected(primary.id, primary.name);

    refreshAlerts();
}

void MainWindow::onSamplesUpdated()
{
    if (m_historyStore->isOpen()) {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        for (int row = 0; row < m_model->rowCount(); ++row) {
            const PatientVitals vitals = m_model->vitalsAt(row);
            m_historyStore->recordSample(vitals.id, now, vitals.heartRate, vitals.spo2, vitals.systolic,
                                          vitals.diastolic);
        }
    }
    m_trendChart->refresh();
    refreshAlerts();
}

void MainWindow::onPatientSelected(const QString& patientId, const QString& name)
{
    m_trendChart->setSelectedPatient(patientId, name);
}

void MainWindow::refreshAlerts()
{
    struct Alert {
        int severity;
        QString text;
    };
    std::vector<Alert> alerts;

    for (int row = 0; row < m_model->rowCount(); ++row) {
        const AlarmSeverity severity = m_model->severityAt(row);
        if (severity == AlarmSeverity::Normal) {
            continue;
        }
        const PatientVitals vitals = m_model->vitalsAt(row);
        const QString label = severity == AlarmSeverity::Critical ? tr("CRITICAL") : tr("Warning");
        alerts.push_back({static_cast<int>(severity),
                           QStringLiteral("[%1] %2 - HR %3 bpm, SpO2 %4%")
                               .arg(label, vitals.name)
                               .arg(qRound(vitals.heartRate))
                               .arg(qRound(vitals.spo2))});
    }

    std::sort(alerts.begin(), alerts.end(), [](const Alert& a, const Alert& b) { return a.severity > b.severity; });

    m_alertList->clear();
    for (const Alert& alert : alerts) {
        auto* item = new QListWidgetItem(alert.text, m_alertList);
        item->setForeground(alert.severity == static_cast<int>(AlarmSeverity::Critical) ? QColor("#ff6b6b")
                                                                                          : QColor("#ffcc66"));
    }
    if (alerts.empty()) {
        new QListWidgetItem(tr("No active alerts"), m_alertList);
    }
}

} // namespace qttutorial::medical::telehealth
