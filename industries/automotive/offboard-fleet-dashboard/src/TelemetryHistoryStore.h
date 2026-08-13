// SPDX-License-Identifier: MIT
#pragma once

#include <QSqlDatabase>
#include <QString>

#include <vector>

namespace qttutorial::automotive::fleet {

// Thin wrapper around a SQLite-backed history table, standing in for a fleet
// back-office's telemetry archive. Kept independent of any UI so it can be
// unit tested against an in-memory database.
class TelemetryHistoryStore {
public:
    explicit TelemetryHistoryStore(const QString& databasePath, const QString& connectionName);
    ~TelemetryHistoryStore();

    TelemetryHistoryStore(const TelemetryHistoryStore&) = delete;
    TelemetryHistoryStore& operator=(const TelemetryHistoryStore&) = delete;

    [[nodiscard]] bool isOpen() const;
    bool recordSample(const QString& vehicleId, qint64 timestampMs, double speedKph, double fuelPercent);
    [[nodiscard]] std::vector<double> recentSpeeds(const QString& vehicleId, int limit) const;
    [[nodiscard]] int sampleCount(const QString& vehicleId) const;

private:
    QSqlDatabase m_db;
    QString m_connectionName;
};

} // namespace qttutorial::automotive::fleet
