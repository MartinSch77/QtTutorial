// SPDX-License-Identifier: MIT
#pragma once

#include <QDateTime>
#include <QSqlDatabase>
#include <QString>

#include <vector>

namespace qttutorial::space {

struct TelemetryRecord {
    QString satelliteName;
    double batteryPercent = 0.0;
    double phaseDeg = 0.0;
    bool inEclipse = false;
    QString health;
    QDateTime timestamp;
};

// SQLite-backed telemetry history via QtSql. Every statement is parameterized
// (prepare + bindValue) - no string-built SQL anywhere in this class.
class TelemetryHistoryStore {
public:
    explicit TelemetryHistoryStore(const QString& connectionName);
    ~TelemetryHistoryStore();

    TelemetryHistoryStore(const TelemetryHistoryStore&) = delete;
    TelemetryHistoryStore& operator=(const TelemetryHistoryStore&) = delete;

    [[nodiscard]] bool open(const QString& databasePath);
    [[nodiscard]] bool recordTelemetry(const TelemetryRecord& record);
    [[nodiscard]] std::vector<TelemetryRecord> history(const QString& satelliteName, int limit = 200) const;

private:
    QString m_connectionName;
};

} // namespace qttutorial::space
