// SPDX-License-Identifier: MIT
#include "HaulHistoryStore.h"

#include <QSqlError>
#include <QSqlQuery>

#include <algorithm>

namespace qttutorial::mining::pit {

HaulHistoryStore::HaulHistoryStore(const QString& databasePath, const QString& connectionName)
    : m_db(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName))
    , m_connectionName(connectionName)
{
    m_db.setDatabaseName(databasePath);
    if (!m_db.open()) {
        return;
    }

    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS pit_haul_history ("
            "truck_id TEXT NOT NULL,"
            "timestamp_ms INTEGER NOT NULL,"
            "state_label TEXT NOT NULL,"
            "payload_tonnes REAL NOT NULL,"
            "cumulative_tonnes REAL NOT NULL"
            ")"))) {
        m_db.close();
    }
}

HaulHistoryStore::~HaulHistoryStore()
{
    m_db.close();
    QSqlDatabase::removeDatabase(m_connectionName);
}

bool HaulHistoryStore::isOpen() const
{
    return m_db.isOpen();
}

bool HaulHistoryStore::recordSample(const QString& truckId, qint64 timestampMs, const QString& stateLabel,
                                     double payloadTonnes, double cumulativeTonnesHauled)
{
    if (!m_db.isOpen()) {
        return false;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral(
            "INSERT INTO pit_haul_history (truck_id, timestamp_ms, state_label, payload_tonnes, cumulative_tonnes) "
            "VALUES (?, ?, ?, ?, ?)"))) {
        return false;
    }
    query.addBindValue(truckId);
    query.addBindValue(timestampMs);
    query.addBindValue(stateLabel);
    query.addBindValue(payloadTonnes);
    query.addBindValue(cumulativeTonnesHauled);
    return query.exec();
}

std::vector<double> HaulHistoryStore::recentCumulativeTonnes(const QString& truckId, int limit) const
{
    std::vector<double> values;
    if (!m_db.isOpen()) {
        return values;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral(
            "SELECT cumulative_tonnes FROM pit_haul_history WHERE truck_id = ? "
            "ORDER BY timestamp_ms DESC LIMIT ?"))) {
        return values;
    }
    query.addBindValue(truckId);
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

int HaulHistoryStore::sampleCount(const QString& truckId) const
{
    if (!m_db.isOpen()) {
        return 0;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("SELECT COUNT(*) FROM pit_haul_history WHERE truck_id = ?"))) {
        return 0;
    }
    query.addBindValue(truckId);
    if (!query.exec() || !query.next()) {
        return 0;
    }
    return query.value(0).toInt();
}

} // namespace qttutorial::mining::pit
