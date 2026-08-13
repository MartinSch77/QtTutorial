// SPDX-License-Identifier: MIT
#include "TelemetryHistoryStore.h"

#include <QSqlError>
#include <QSqlQuery>

namespace qttutorial::space {

TelemetryHistoryStore::TelemetryHistoryStore(const QString& connectionName)
    : m_connectionName(connectionName)
{
}

TelemetryHistoryStore::~TelemetryHistoryStore()
{
    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

bool TelemetryHistoryStore::open(const QString& databasePath)
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    db.setDatabaseName(databasePath);
    if (!db.open()) {
        return false;
    }

    QSqlQuery query(db);
    return query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS telemetry_history ("
        "satellite_name TEXT NOT NULL, "
        "battery_percent REAL NOT NULL, "
        "phase_deg REAL NOT NULL, "
        "in_eclipse INTEGER NOT NULL, "
        "health TEXT NOT NULL, "
        "timestamp TEXT NOT NULL)"));
}

bool TelemetryHistoryStore::recordTelemetry(const TelemetryRecord& record)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    if (!query.prepare(QStringLiteral(
            "INSERT INTO telemetry_history (satellite_name, battery_percent, phase_deg, in_eclipse, health, timestamp) "
            "VALUES (:name, :battery, :phase, :eclipse, :health, :timestamp)"))) {
        return false;
    }
    query.bindValue(QStringLiteral(":name"), record.satelliteName);
    query.bindValue(QStringLiteral(":battery"), record.batteryPercent);
    query.bindValue(QStringLiteral(":phase"), record.phaseDeg);
    query.bindValue(QStringLiteral(":eclipse"), record.inEclipse ? 1 : 0);
    query.bindValue(QStringLiteral(":health"), record.health);
    query.bindValue(QStringLiteral(":timestamp"), record.timestamp.toString(Qt::ISODateWithMs));
    return query.exec();
}

std::vector<TelemetryRecord> TelemetryHistoryStore::history(const QString& satelliteName, int limit) const
{
    std::vector<TelemetryRecord> result;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    if (!query.prepare(QStringLiteral(
            "SELECT satellite_name, battery_percent, phase_deg, in_eclipse, health, timestamp FROM telemetry_history "
            "WHERE satellite_name = :name ORDER BY timestamp DESC LIMIT :limit"))) {
        return result;
    }
    query.bindValue(QStringLiteral(":name"), satelliteName);
    query.bindValue(QStringLiteral(":limit"), limit);
    if (!query.exec()) {
        return result;
    }

    while (query.next()) {
        TelemetryRecord record;
        record.satelliteName = query.value(0).toString();
        record.batteryPercent = query.value(1).toDouble();
        record.phaseDeg = query.value(2).toDouble();
        record.inEclipse = query.value(3).toInt() != 0;
        record.health = query.value(4).toString();
        record.timestamp = QDateTime::fromString(query.value(5).toString(), Qt::ISODateWithMs);
        result.push_back(record);
    }
    return result;
}

} // namespace qttutorial::space
