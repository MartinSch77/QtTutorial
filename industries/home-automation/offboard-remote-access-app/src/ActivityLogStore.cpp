// SPDX-License-Identifier: MIT
#include "ActivityLogStore.h"

#include <QSqlQuery>

namespace qttutorial::homeautomation::remote {

ActivityLogStore::ActivityLogStore(const QString& databasePath, const QString& connectionName)
    : m_db(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName))
    , m_connectionName(connectionName)
{
    m_db.setDatabaseName(databasePath);
    if (!m_db.open()) {
        return;
    }

    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS activity_log ("
            "description TEXT NOT NULL,"
            "timestamp_ms INTEGER NOT NULL"
            ")"))) {
        m_db.close();
    }
}

ActivityLogStore::~ActivityLogStore()
{
    m_db.close();
    QSqlDatabase::removeDatabase(m_connectionName);
}

bool ActivityLogStore::isOpen() const
{
    return m_db.isOpen();
}

bool ActivityLogStore::record(const QString& description, qint64 timestampMs)
{
    if (!m_db.isOpen()) {
        return false;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("INSERT INTO activity_log (description, timestamp_ms) VALUES (?, ?)"))) {
        return false;
    }
    query.addBindValue(description);
    query.addBindValue(timestampMs);
    return query.exec();
}

std::vector<ActivityLogEntry> ActivityLogStore::recentEntries(int limit) const
{
    std::vector<ActivityLogEntry> entries;
    if (!m_db.isOpen()) {
        return entries;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("SELECT description, timestamp_ms FROM activity_log ORDER BY timestamp_ms DESC LIMIT ?"))) {
        return entries;
    }
    query.addBindValue(limit);
    if (!query.exec()) {
        return entries;
    }
    while (query.next()) {
        entries.push_back(ActivityLogEntry{.description = query.value(0).toString(), .timestampMs = query.value(1).toLongLong()});
    }
    return entries;
}

int ActivityLogStore::entryCount() const
{
    if (!m_db.isOpen()) {
        return 0;
    }
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral("SELECT COUNT(*) FROM activity_log")) || !query.next()) {
        return 0;
    }
    return query.value(0).toInt();
}

} // namespace qttutorial::homeautomation::remote
