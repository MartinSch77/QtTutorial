// SPDX-License-Identifier: MIT
#include "AssetHistoryStore.h"

#include <QSqlError>
#include <QSqlQuery>

namespace qttutorial::defence {

namespace {
AssetType assetTypeFromString(const QString& value)
{
    if (value == QStringLiteral("Drone")) {
        return AssetType::Drone;
    }
    if (value == QStringLiteral("Vessel")) {
        return AssetType::Vessel;
    }
    if (value == QStringLiteral("Installation")) {
        return AssetType::Installation;
    }
    return AssetType::Vehicle;
}
}

AssetHistoryStore::AssetHistoryStore(const QString& connectionName)
    : m_connectionName(connectionName)
{
}

AssetHistoryStore::~AssetHistoryStore()
{
    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

bool AssetHistoryStore::open(const QString& databasePath)
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    db.setDatabaseName(databasePath);
    if (!db.open()) {
        return false;
    }

    QSqlQuery query(db);
    return query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS asset_history ("
        "asset_id TEXT NOT NULL, "
        "asset_type TEXT NOT NULL, "
        "x_km REAL NOT NULL, "
        "y_km REAL NOT NULL, "
        "heading_deg REAL NOT NULL, "
        "health TEXT NOT NULL, "
        "timestamp TEXT NOT NULL)"));
}

bool AssetHistoryStore::recordStatus(const AssetHistoryRecord& record)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    if (!query.prepare(QStringLiteral(
            "INSERT INTO asset_history (asset_id, asset_type, x_km, y_km, heading_deg, health, timestamp) "
            "VALUES (:id, :type, :x, :y, :heading, :health, :timestamp)"))) {
        return false;
    }
    query.bindValue(QStringLiteral(":id"), record.assetId);
    query.bindValue(QStringLiteral(":type"), toString(record.type));
    query.bindValue(QStringLiteral(":x"), record.xKm);
    query.bindValue(QStringLiteral(":y"), record.yKm);
    query.bindValue(QStringLiteral(":heading"), record.headingDeg);
    query.bindValue(QStringLiteral(":health"), record.health);
    query.bindValue(QStringLiteral(":timestamp"), record.timestamp.toString(Qt::ISODateWithMs));
    return query.exec();
}

std::vector<AssetHistoryRecord> AssetHistoryStore::history(const QString& assetId, int limit) const
{
    std::vector<AssetHistoryRecord> result;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    if (!query.prepare(QStringLiteral(
            "SELECT asset_id, asset_type, x_km, y_km, heading_deg, health, timestamp FROM asset_history "
            "WHERE asset_id = :id ORDER BY timestamp DESC LIMIT :limit"))) {
        return result;
    }
    query.bindValue(QStringLiteral(":id"), assetId);
    query.bindValue(QStringLiteral(":limit"), limit);
    if (!query.exec()) {
        return result;
    }

    while (query.next()) {
        AssetHistoryRecord record;
        record.assetId = query.value(0).toString();
        record.type = assetTypeFromString(query.value(1).toString());
        record.xKm = query.value(2).toDouble();
        record.yKm = query.value(3).toDouble();
        record.headingDeg = query.value(4).toDouble();
        record.health = query.value(5).toString();
        record.timestamp = QDateTime::fromString(query.value(6).toString(), Qt::ISODateWithMs);
        result.push_back(record);
    }
    return result;
}

} // namespace qttutorial::defence
