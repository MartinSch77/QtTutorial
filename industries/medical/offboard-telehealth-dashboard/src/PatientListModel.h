// SPDX-License-Identifier: MIT
#pragma once

#include "PatientVitalsSimulator.h"

#include <QAbstractListModel>
#include <QTimer>

#include <vector>

namespace qttutorial::medical::telehealth {

// A QAbstractListModel aggregating several simulated remote patient monitors,
// each with a rolling heart-rate trend buffer used by SparklineDelegate.
class PatientListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Role {
        IdRole = Qt::UserRole + 1,
        NameRole,
        HeartRateRole,
        Spo2Role,
        BloodPressureRole,
        SeverityRole,
        TrendRole,
    };

    explicit PatientListModel(int patientCount = 5, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] PatientVitals vitalsAt(int row) const;
    [[nodiscard]] AlarmSeverity severityAt(int row) const;

signals:
    void samplesUpdated();

private:
    struct PatientRow {
        PatientVitals vitals;
        AlarmSeverity severity = AlarmSeverity::Normal;
        std::vector<double> trend;
    };

    void tick();

    int m_patientCount;
    double m_elapsedSeconds = 0.0;
    std::vector<PatientRow> m_rows;
    QTimer m_timer;
};

} // namespace qttutorial::medical::telehealth
