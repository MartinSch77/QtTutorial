// SPDX-License-Identifier: MIT
#pragma once

#include <QSqlDatabase>
#include <QString>

#include <vector>

namespace qttutorial::agriculture::ops {

// Thin wrapper around a SQLite-backed history table, standing in for a farm
// operations back office's pass-completion archive. Kept independent of any
// UI so it can be unit tested against an in-memory database.
class OperationHistoryStore {
public:
    explicit OperationHistoryStore(const QString& databasePath, const QString& connectionName);
    ~OperationHistoryStore();

    OperationHistoryStore(const OperationHistoryStore&) = delete;
    OperationHistoryStore& operator=(const OperationHistoryStore&) = delete;

    [[nodiscard]] bool isOpen() const;
    bool recordSample(const QString& fieldId, qint64 timestampMs, double coveragePercent, const QString& status);
    [[nodiscard]] std::vector<double> recentCoverage(const QString& fieldId, int limit) const;
    [[nodiscard]] int sampleCount(const QString& fieldId) const;

private:
    QSqlDatabase m_db;
    QString m_connectionName;
};

} // namespace qttutorial::agriculture::ops
