// SPDX-License-Identifier: MIT
#include "TelemetryHistoryStore.h"

#include <QSqlError>
#include <QSqlQuery>

#include <algorithm>

namespace qttutorial::two_wheelers::fleet {

TelemetryHistoryStore::TelemetryHistoryStore(const QString& databasePath, const QString& connectionName)
    : m_db(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName))
    , m_connectionName(connectionName)
{
    m_db.setDatabaseName(databasePath);
    if (!m_db.open()) {
        return;
    }

    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS two_wheelers_fleet_history ("
            "vehicle_id TEXT NOT NULL,"
            "timestamp_ms INTEGER NOT NULL,"
            "speed_kph REAL NOT NULL,"
            "battery_percent REAL NOT NULL"
            ")"))) {
        m_db.close();
    }
}

TelemetryHistoryStore::~TelemetryHistoryStore()
{
    m_db.close();
    QSqlDatabase::removeDatabase(m_connectionName);
}

bool TelemetryHistoryStore::isOpen() const
{
    return m_db.isOpen();
}

bool TelemetryHistoryStore::recordSample(const QString& vehicleId, qint64 timestampMs, double speedKph,
                                          double batteryPercent)
{
    if (!m_db.isOpen()) {
        return false;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral(
            "INSERT INTO two_wheelers_fleet_history (vehicle_id, timestamp_ms, speed_kph, battery_percent) "
            "VALUES (?, ?, ?, ?)"))) {
        return false;
    }
    query.addBindValue(vehicleId);
    query.addBindValue(timestampMs);
    query.addBindValue(speedKph);
    query.addBindValue(batteryPercent);
    return query.exec();
}

std::vector<double> TelemetryHistoryStore::recentSpeeds(const QString& vehicleId, int limit) const
{
    std::vector<double> speeds;
    if (!m_db.isOpen()) {
        return speeds;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral(
            "SELECT speed_kph FROM two_wheelers_fleet_history WHERE vehicle_id = ? "
            "ORDER BY timestamp_ms DESC LIMIT ?"))) {
        return speeds;
    }
    query.addBindValue(vehicleId);
    query.addBindValue(limit);
    if (!query.exec()) {
        return speeds;
    }
    while (query.next()) {
        speeds.push_back(query.value(0).toDouble());
    }
    std::reverse(speeds.begin(), speeds.end());
    return speeds;
}

int TelemetryHistoryStore::sampleCount(const QString& vehicleId) const
{
    if (!m_db.isOpen()) {
        return 0;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("SELECT COUNT(*) FROM two_wheelers_fleet_history WHERE vehicle_id = ?"))) {
        return 0;
    }
    query.addBindValue(vehicleId);
    if (!query.exec() || !query.next()) {
        return 0;
    }
    return query.value(0).toInt();
}

} // namespace qttutorial::two_wheelers::fleet
