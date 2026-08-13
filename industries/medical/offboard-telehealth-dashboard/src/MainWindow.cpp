// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include "SparklineDelegate.h"

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
    , m_patientList(new QListView(this))
    , m_alertList(new QListWidget(this))
    , m_historyStore(std::make_unique<VitalsHistoryStore>(QStringLiteral(":memory:"),
                                                           QStringLiteral("telehealth_dashboard_history")))
{
    setWindowTitle(tr("QtTutorial - Telehealth Dashboard"));

    m_patientList->setModel(m_model);
    m_patientList->setItemDelegate(new SparklineDelegate(this));
    m_patientList->setUniformItemSizes(false);

    auto* splitter = new QSplitter(Qt::Vertical, this);
    auto* patientContainer = new QWidget(this);
    auto* patientLayout = new QVBoxLayout(patientContainer);
    patientLayout->addWidget(new QLabel(tr("Patients"), this));
    patientLayout->addWidget(m_patientList);
    splitter->addWidget(patientContainer);

    auto* alertContainer = new QWidget(this);
    auto* alertLayout = new QVBoxLayout(alertContainer);
    alertLayout->addWidget(new QLabel(tr("Alerts (most severe first)"), this));
    alertLayout->addWidget(m_alertList);
    splitter->addWidget(alertContainer);

    setCentralWidget(splitter);
    resize(720, 640);

    connect(m_model, &PatientListModel::samplesUpdated, this, &MainWindow::onSamplesUpdated);
    refreshAlerts();
}

void MainWindow::onSamplesUpdated()
{
    if (m_historyStore->isOpen()) {
        const PatientVitals primary = m_model->vitalsAt(kPrimaryPatientRow);
        m_historyStore->recordSample(primary.id, QDateTime::currentMSecsSinceEpoch(), primary.heartRate,
                                      primary.spo2, primary.systolic, primary.diastolic);
    }
    refreshAlerts();
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
