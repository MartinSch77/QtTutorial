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
            "status TEXT NOT NULL"
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
                                          const QString& status)
{
    if (!m_db.isOpen()) {
        return false;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral(
            "INSERT INTO operation_history (field_id, timestamp_ms, coverage_percent, status) "
            "VALUES (?, ?, ?, ?)"))) {
        return false;
    }
    query.addBindValue(fieldId);
    query.addBindValue(timestampMs);
    query.addBindValue(coveragePercent);
    query.addBindValue(status);
    return query.exec();
}

std::vector<double> OperationHistoryStore::recentCoverage(const QString& fieldId, int limit) const
{
    std::vector<double> coverage;
    if (!m_db.isOpen()) {
        return coverage;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral(
            "SELECT coverage_percent FROM operation_history WHERE field_id = ? "
            "ORDER BY timestamp_ms DESC LIMIT ?"))) {
        return coverage;
    }
    query.addBindValue(fieldId);
    query.addBindValue(limit);
    if (!query.exec()) {
        return coverage;
    }
    while (query.next()) {
        coverage.push_back(query.value(0).toDouble());
    }
    std::reverse(coverage.begin(), coverage.end());
    return coverage;
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
