// SPDX-License-Identifier: MIT
#include "PatientListModel.h"

#include <QVariant>

namespace qttutorial::medical::telehealth {

namespace {
constexpr std::size_t kTrendBufferSize = 30;
}

PatientListModel::PatientListModel(int patientCount, QObject* parent)
    : QAbstractListModel(parent)
    , m_patientCount(patientCount)
{
    m_rows.resize(static_cast<std::size_t>(patientCount));
    for (int i = 0; i < patientCount; ++i) {
        PatientRow& row = m_rows[static_cast<std::size_t>(i)];
        row.vitals = PatientVitalsSimulator::sampleAt(i, 0.0);
        row.severity = PatientVitalsSimulator::classify(row.vitals);
        row.trend.push_back(row.vitals.heartRate);
    }

    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &PatientListModel::tick);
    m_timer.start();
}

void PatientListModel::tick()
{
    m_elapsedSeconds += 1.0;
    for (int i = 0; i < m_patientCount; ++i) {
        PatientRow& row = m_rows[static_cast<std::size_t>(i)];
        row.vitals = PatientVitalsSimulator::sampleAt(i, m_elapsedSeconds);
        row.severity = PatientVitalsSimulator::classify(row.vitals);
        row.trend.push_back(row.vitals.heartRate);
        if (row.trend.size() > kTrendBufferSize) {
            row.trend.erase(row.trend.begin());
        }
    }
    emit dataChanged(index(0), index(m_patientCount - 1));
    emit samplesUpdated();
}

int PatientListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_patientCount;
}

QVariant PatientListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_patientCount) {
        return {};
    }
    const PatientRow& row = m_rows[static_cast<std::size_t>(index.row())];
    switch (role) {
    case IdRole:
        return row.vitals.id;
    case NameRole:
        return row.vitals.name;
    case HeartRateRole:
        return row.vitals.heartRate;
    case Spo2Role:
        return row.vitals.spo2;
    case BloodPressureRole:
        return QStringLiteral("%1/%2").arg(qRound(row.vitals.systolic)).arg(qRound(row.vitals.diastolic));
    case SeverityRole:
        return static_cast<int>(row.severity);
    case TrendRole: {
        QVariantList trend;
        for (double v : row.trend) {
            trend.append(v);
        }
        return trend;
    }
    default:
        return {};
    }
}

QHash<int, QByteArray> PatientListModel::roleNames() const
{
    return {
        {IdRole, "patientId"},
        {NameRole, "name"},
        {HeartRateRole, "heartRate"},
        {Spo2Role, "spo2"},
        {BloodPressureRole, "bloodPressure"},
        {SeverityRole, "severity"},
        {TrendRole, "trend"},
    };
}

PatientVitals PatientListModel::vitalsAt(int row) const
{
    if (row < 0 || row >= m_patientCount) {
        return {};
    }
    return m_rows[static_cast<std::size_t>(row)].vitals;
}

AlarmSeverity PatientListModel::severityAt(int row) const
{
    if (row < 0 || row >= m_patientCount) {
        return AlarmSeverity::Normal;
    }
    return m_rows[static_cast<std::size_t>(row)].severity;
}

} // namespace qttutorial::medical::telehealth
