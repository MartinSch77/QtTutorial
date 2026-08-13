// SPDX-License-Identifier: MIT
#pragma once

#include <QDateTime>
#include <QSqlDatabase>
#include <QString>

#include <utility>
#include <vector>

namespace qttutorial::fleet_ops {

struct RunSample {
    QDateTime timestamp;
    double positionKm = 0.0;
    double speedKmh = 0.0;
    double delayMinutes = 0.0;
};

// SQLite-backed run history for the fleet: a real schema (train_id,
// timestamp, position_km, speed_kmh, delay_minutes), parameterized
// statements, and a time-ranged query so punctuality trends can be pulled
// back out per train. Each instance opens its own uniquely named QSqlDatabase
// connection so several can coexist (e.g. one per test).
class RunHistoryStore {
public:
    explicit RunHistoryStore(const QString& connectionName, const QString& databasePath = QStringLiteral(":memory:"));
    ~RunHistoryStore();

    RunHistoryStore(const RunHistoryStore&) = delete;
    RunHistoryStore& operator=(const RunHistoryStore&) = delete;

    [[nodiscard]] bool isOpen() const;
    bool createSchema();

    bool insertSample(const QString& trainId, const RunSample& sample);

    [[nodiscard]] std::vector<RunSample> samplesInRange(const QString& trainId, const QDateTime& from,
                                                         const QDateTime& to) const;

    [[nodiscard]] double averageDelayInRange(const QDateTime& from, const QDateTime& to) const;

    // The fleet-wide average delay, bucketed into `bucketSeconds`-wide
    // windows across [from, to] — the basis for a network-wide punctuality
    // trend (as opposed to `averageDelayInRange`'s single scalar), returned
    // as (bucket start time, average delay minutes across all trains in
    // that bucket) pairs, in order, omitting empty buckets.
    [[nodiscard]] std::vector<std::pair<QDateTime, double>> networkDelaySeries(const QDateTime& from,
                                                                                const QDateTime& to,
                                                                                qint64 bucketSeconds) const;

private:
    QString m_connectionName;
};

} // namespace qttutorial::fleet_ops
