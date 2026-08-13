// SPDX-License-Identifier: MIT
#pragma once

#include <QDateTime>
#include <QSqlDatabase>
#include <QString>

#include <vector>

namespace qttutorial::avionics {

struct ParameterSample {
    QString aircraftTail;
    QString parameterName;
    double value = 0.0;
    QDateTime timestamp;
};

// A thin QtSql wrapper around a SQLite-backed flight/parameter history table.
// Every statement is parameterized (prepare + bindValue) rather than
// string-concatenated, which is the point being demonstrated here as much as the
// persistence itself.
class FlightHistoryStore {
public:
    explicit FlightHistoryStore(const QString& connectionName);
    ~FlightHistoryStore();

    FlightHistoryStore(const FlightHistoryStore&) = delete;
    FlightHistoryStore& operator=(const FlightHistoryStore&) = delete;

    [[nodiscard]] bool open(const QString& databasePath);
    [[nodiscard]] bool recordSample(const ParameterSample& sample);
    [[nodiscard]] std::vector<ParameterSample> history(const QString& aircraftTail, const QString& parameterName,
                                                         int limit = 100) const;

private:
    QString m_connectionName;
};

} // namespace qttutorial::avionics
