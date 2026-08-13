// SPDX-License-Identifier: MIT
#include "RunHistoryStore.h"

#include <QSqlQuery>

namespace qttutorial::fleet_ops {

RunHistoryStore::RunHistoryStore(const QString& connectionName, const QString& databasePath)
    : m_connectionName(connectionName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    db.setDatabaseName(databasePath);
    if (!db.open()) {
        return;
    }
}

RunHistoryStore::~RunHistoryStore()
{
    QSqlDatabase::database(m_connectionName).close();
    QSqlDatabase::removeDatabase(m_connectionName);
}

bool RunHistoryStore::isOpen() const
{
    return QSqlDatabase::database(m_connectionName).isOpen();
}

bool RunHistoryStore::createSchema()
{
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    return query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS run_history ("
                                      "train_id TEXT NOT NULL,"
                                      "timestamp INTEGER NOT NULL,"
                                      "position_km REAL NOT NULL,"
                                      "speed_kmh REAL NOT NULL,"
                                      "delay_minutes REAL NOT NULL)"))
        && query.exec(QStringLiteral("CREATE INDEX IF NOT EXISTS idx_run_history_train_time "
                                      "ON run_history(train_id, timestamp)"));
}

bool RunHistoryStore::insertSample(const QString& trainId, const RunSample& sample)
{
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    if (!query.prepare(QStringLiteral("INSERT INTO run_history "
                                       "(train_id, timestamp, position_km, speed_kmh, delay_minutes) "
                                       "VALUES (:train, :ts, :position, :speed, :delay)"))) {
        return false;
    }
    query.bindValue(QStringLiteral(":train"), trainId);
    query.bindValue(QStringLiteral(":ts"), sample.timestamp.toMSecsSinceEpoch());
    query.bindValue(QStringLiteral(":position"), sample.positionKm);
    query.bindValue(QStringLiteral(":speed"), sample.speedKmh);
    query.bindValue(QStringLiteral(":delay"), sample.delayMinutes);
    return query.exec();
}

std::vector<RunSample> RunHistoryStore::samplesInRange(const QString& trainId, const QDateTime& from,
                                                        const QDateTime& to) const
{
    std::vector<RunSample> samples;
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    if (!query.prepare(QStringLiteral("SELECT timestamp, position_km, speed_kmh, delay_minutes FROM run_history "
                                       "WHERE train_id = :train AND timestamp BETWEEN :from AND :to "
                                       "ORDER BY timestamp ASC"))) {
        return samples;
    }
    query.bindValue(QStringLiteral(":train"), trainId);
    query.bindValue(QStringLiteral(":from"), from.toMSecsSinceEpoch());
    query.bindValue(QStringLiteral(":to"), to.toMSecsSinceEpoch());
    if (!query.exec()) {
        return samples;
    }
    while (query.next()) {
        RunSample sample;
        sample.timestamp = QDateTime::fromMSecsSinceEpoch(query.value(0).toLongLong());
        sample.positionKm = query.value(1).toDouble();
        sample.speedKmh = query.value(2).toDouble();
        sample.delayMinutes = query.value(3).toDouble();
        samples.push_back(sample);
    }
    return samples;
}

double RunHistoryStore::averageDelayInRange(const QDateTime& from, const QDateTime& to) const
{
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    if (!query.prepare(QStringLiteral("SELECT AVG(delay_minutes) FROM run_history "
                                       "WHERE timestamp BETWEEN :from AND :to"))) {
        return 0.0;
    }
    query.bindValue(QStringLiteral(":from"), from.toMSecsSinceEpoch());
    query.bindValue(QStringLiteral(":to"), to.toMSecsSinceEpoch());
    if (!query.exec() || !query.next()) {
        return 0.0;
    }
    return query.value(0).toDouble();
}

std::vector<std::pair<QDateTime, double>> RunHistoryStore::networkDelaySeries(const QDateTime& from,
                                                                                const QDateTime& to,
                                                                                qint64 bucketSeconds) const
{
    std::vector<std::pair<QDateTime, double>> series;
    if (bucketSeconds <= 0) {
        return series;
    }
    const qint64 bucketMs = bucketSeconds * 1000;
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    if (!query.prepare(QStringLiteral("SELECT (timestamp / :bucket) * :bucket AS bucket_start, "
                                       "AVG(delay_minutes) FROM run_history "
                                       "WHERE timestamp BETWEEN :from AND :to "
                                       "GROUP BY bucket_start ORDER BY bucket_start ASC"))) {
        return series;
    }
    query.bindValue(QStringLiteral(":bucket"), bucketMs);
    query.bindValue(QStringLiteral(":from"), from.toMSecsSinceEpoch());
    query.bindValue(QStringLiteral(":to"), to.toMSecsSinceEpoch());
    if (!query.exec()) {
        return series;
    }
    while (query.next()) {
        const QDateTime bucketStart = QDateTime::fromMSecsSinceEpoch(query.value(0).toLongLong());
        series.emplace_back(bucketStart, query.value(1).toDouble());
    }
    return series;
}

} // namespace qttutorial::fleet_ops
