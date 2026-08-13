// SPDX-License-Identifier: MIT
#pragma once

#include <QSqlDatabase>
#include <QString>

#include <vector>

namespace qttutorial::mining::pit {

// Thin wrapper around a SQLite-backed history table, standing in for a pit
// operations back office's haul-cycle archive. Kept independent of any UI so
// it can be unit tested against an in-memory database.
class HaulHistoryStore {
public:
    explicit HaulHistoryStore(const QString& databasePath, const QString& connectionName);
    ~HaulHistoryStore();

    HaulHistoryStore(const HaulHistoryStore&) = delete;
    HaulHistoryStore& operator=(const HaulHistoryStore&) = delete;

    [[nodiscard]] bool isOpen() const;
    bool recordSample(const QString& truckId, qint64 timestampMs, const QString& stateLabel,
                       double payloadTonnes, double cumulativeTonnesHauled);
    [[nodiscard]] std::vector<double> recentCumulativeTonnes(const QString& truckId, int limit) const;
    [[nodiscard]] int sampleCount(const QString& truckId) const;

private:
    QSqlDatabase m_db;
    QString m_connectionName;
};

} // namespace qttutorial::mining::pit
