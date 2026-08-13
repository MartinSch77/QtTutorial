// SPDX-License-Identifier: MIT
#pragma once

#include <QSqlDatabase>
#include <QString>

#include <vector>

namespace qttutorial::medical::telehealth {

// SQLite-backed vitals archive for a single patient's history, standing in
// for a telehealth back office's long-term store. Independent of any UI so
// it can be unit tested against an in-memory database.
class VitalsHistoryStore {
public:
    explicit VitalsHistoryStore(const QString& databasePath, const QString& connectionName);
    ~VitalsHistoryStore();

    VitalsHistoryStore(const VitalsHistoryStore&) = delete;
    VitalsHistoryStore& operator=(const VitalsHistoryStore&) = delete;

    [[nodiscard]] bool isOpen() const;
    bool recordSample(const QString& patientId, qint64 timestampMs, double heartRate, double spo2,
                       double systolic, double diastolic);
    [[nodiscard]] std::vector<double> recentHeartRates(const QString& patientId, int limit) const;
    [[nodiscard]] int sampleCount(const QString& patientId) const;

private:
    QSqlDatabase m_db;
    QString m_connectionName;
};

} // namespace qttutorial::medical::telehealth
