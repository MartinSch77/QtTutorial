// SPDX-License-Identifier: MIT
#include "Historian.h"

#include <QSqlError>
#include <QSqlQuery>

namespace qttutorial::plant_scada {

Historian::Historian(const QString& connectionName, const QString& databasePath)
    : m_connectionName(connectionName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    db.setDatabaseName(databasePath);
    if (!db.open()) {
        return;
    }
}

Historian::~Historian()
{
    QSqlDatabase::database(m_connectionName).close();
    QSqlDatabase::removeDatabase(m_connectionName);
}

bool Historian::isOpen() const
{
    return QSqlDatabase::database(m_connectionName).isOpen();
}

bool Historian::createSchema()
{
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    return query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS samples ("
                                      "tag_id TEXT NOT NULL,"
                                      "timestamp INTEGER NOT NULL,"
                                      "value REAL NOT NULL)"))
        && query.exec(QStringLiteral("CREATE INDEX IF NOT EXISTS idx_samples_tag_time "
                                      "ON samples(tag_id, timestamp)"));
}

bool Historian::insertSample(const QString& tagId, const QDateTime& timestamp, double value)
{
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    if (!query.prepare(QStringLiteral("INSERT INTO samples (tag_id, timestamp, value) VALUES (:tag, :ts, :value)"))) {
        return false;
    }
    query.bindValue(QStringLiteral(":tag"), tagId);
    query.bindValue(QStringLiteral(":ts"), timestamp.toMSecsSinceEpoch());
    query.bindValue(QStringLiteral(":value"), value);
    return query.exec();
}

std::vector<Sample> Historian::samplesInRange(const QString& tagId, const QDateTime& from, const QDateTime& to) const
{
    std::vector<Sample> samples;
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    if (!query.prepare(QStringLiteral("SELECT timestamp, value FROM samples "
                                       "WHERE tag_id = :tag AND timestamp BETWEEN :from AND :to "
                                       "ORDER BY timestamp ASC"))) {
        return samples;
    }
    query.bindValue(QStringLiteral(":tag"), tagId);
    query.bindValue(QStringLiteral(":from"), from.toMSecsSinceEpoch());
    query.bindValue(QStringLiteral(":to"), to.toMSecsSinceEpoch());
    if (!query.exec()) {
        return samples;
    }
    while (query.next()) {
        const qint64 ms = query.value(0).toLongLong();
        samples.push_back(Sample{QDateTime::fromMSecsSinceEpoch(ms), query.value(1).toDouble()});
    }
    return samples;
}

std::optional<Sample> Historian::latestSample(const QString& tagId) const
{
    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    if (!query.prepare(QStringLiteral("SELECT timestamp, value FROM samples "
                                       "WHERE tag_id = :tag ORDER BY timestamp DESC LIMIT 1"))) {
        return std::nullopt;
    }
    query.bindValue(QStringLiteral(":tag"), tagId);
    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }
    const qint64 ms = query.value(0).toLongLong();
    return Sample{QDateTime::fromMSecsSinceEpoch(ms), query.value(1).toDouble()};
}

} // namespace qttutorial::plant_scada
