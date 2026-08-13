// SPDX-License-Identifier: MIT
#include "OperationHistoryStore.h"

#include <QSqlError>
#include <QSqlQuery>

#include <algorithm>

namespace qttutorial::agriculture::ops {

OperationHistoryStore::OperationHistoryStore(const QString& databasePath, const QString& connectionName)
    : m_db(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName))
    , m_connectionName(connectionName)
{
    m_db.setDatabaseName(databasePath);
    if (!m_db.open()) {
        return;
    }

    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS operation_history ("
            "field_id TEXT NOT NULL,"
            "timestamp_ms INTEGER NOT NULL,"
            "coverage_percent REAL NOT NULL,"
            "status TEXT NOT NULL,"
            "engine_load_percent REAL NOT NULL DEFAULT -1,"
            "fuel_level_percent REAL NOT NULL DEFAULT -1"
            ")"))) {
        m_db.close();
    }
}

OperationHistoryStore::~OperationHistoryStore()
{
    m_db.close();
    QSqlDatabase::removeDatabase(m_connectionName);
}

bool OperationHistoryStore::isOpen() const
{
    return m_db.isOpen();
}

bool OperationHistoryStore::recordSample(const QString& fieldId, qint64 timestampMs, double coveragePercent,
                                          const QString& status, double engineLoadPercent, double fuelLevelPercent)
{
    if (!m_db.isOpen()) {
        return false;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral(
            "INSERT INTO operation_history "
            "(field_id, timestamp_ms, coverage_percent, status, engine_load_percent, fuel_level_percent) "
            "VALUES (?, ?, ?, ?, ?, ?)"))) {
        return false;
    }
    query.addBindValue(fieldId);
    query.addBindValue(timestampMs);
    query.addBindValue(coveragePercent);
    query.addBindValue(status);
    query.addBindValue(engineLoadPercent);
    query.addBindValue(fuelLevelPercent);
    return query.exec();
}

namespace {

std::vector<double> recentColumn(const QSqlDatabase& db, const QString& column, const QString& fieldId, int limit)
{
    std::vector<double> values;
    if (!db.isOpen()) {
        return values;
    }
    QSqlQuery query(db);
    if (!query.prepare(QStringLiteral("SELECT %1 FROM operation_history WHERE field_id = ? "
                                       "ORDER BY timestamp_ms DESC LIMIT ?")
                            .arg(column))) {
        return values;
    }
    query.addBindValue(fieldId);
    query.addBindValue(limit);
    if (!query.exec()) {
        return values;
    }
    while (query.next()) {
        values.push_back(query.value(0).toDouble());
    }
    std::reverse(values.begin(), values.end());
    return values;
}

} // namespace

std::vector<double> OperationHistoryStore::recentCoverage(const QString& fieldId, int limit) const
{
    return recentColumn(m_db, QStringLiteral("coverage_percent"), fieldId, limit);
}

std::vector<double> OperationHistoryStore::recentFuelLevels(const QString& fieldId, int limit) const
{
    return recentColumn(m_db, QStringLiteral("fuel_level_percent"), fieldId, limit);
}

std::vector<double> OperationHistoryStore::recentEngineLoads(const QString& fieldId, int limit) const
{
    return recentColumn(m_db, QStringLiteral("engine_load_percent"), fieldId, limit);
}

int OperationHistoryStore::sampleCount(const QString& fieldId) const
{
    if (!m_db.isOpen()) {
        return 0;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("SELECT COUNT(*) FROM operation_history WHERE field_id = ?"))) {
        return 0;
    }
    query.addBindValue(fieldId);
    if (!query.exec() || !query.next()) {
        return 0;
    }
    return query.value(0).toInt();
}

} // namespace qttutorial::agriculture::ops
