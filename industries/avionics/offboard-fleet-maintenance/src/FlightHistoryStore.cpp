// SPDX-License-Identifier: MIT
#include "FlightHistoryStore.h"

#include <QSqlError>
#include <QSqlQuery>

namespace qttutorial::avionics {

FlightHistoryStore::FlightHistoryStore(const QString& connectionName)
    : m_connectionName(connectionName)
{
}

FlightHistoryStore::~FlightHistoryStore()
{
    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

bool FlightHistoryStore::open(const QString& databasePath)
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    db.setDatabaseName(databasePath);
    if (!db.open()) {
        return false;
    }

    QSqlQuery query(db);
    return query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS parameter_history ("
        "aircraft_tail TEXT NOT NULL, "
        "parameter_name TEXT NOT NULL, "
        "value REAL NOT NULL, "
        "timestamp TEXT NOT NULL)"));
}

bool FlightHistoryStore::recordSample(const ParameterSample& sample)
{
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    if (!query.prepare(QStringLiteral(
            "INSERT INTO parameter_history (aircraft_tail, parameter_name, value, timestamp) "
            "VALUES (:tail, :param, :value, :timestamp)"))) {
        return false;
    }
    query.bindValue(QStringLiteral(":tail"), sample.aircraftTail);
    query.bindValue(QStringLiteral(":param"), sample.parameterName);
    query.bindValue(QStringLiteral(":value"), sample.value);
    query.bindValue(QStringLiteral(":timestamp"), sample.timestamp.toString(Qt::ISODateWithMs));
    return query.exec();
}

std::vector<ParameterSample> FlightHistoryStore::history(const QString& aircraftTail, const QString& parameterName,
                                                            int limit) const
{
    std::vector<ParameterSample> result;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    if (!query.prepare(QStringLiteral(
            "SELECT aircraft_tail, parameter_name, value, timestamp FROM parameter_history "
            "WHERE aircraft_tail = :tail AND parameter_name = :param "
            "ORDER BY timestamp DESC LIMIT :limit"))) {
        return result;
    }
    query.bindValue(QStringLiteral(":tail"), aircraftTail);
    query.bindValue(QStringLiteral(":param"), parameterName);
    query.bindValue(QStringLiteral(":limit"), limit);
    if (!query.exec()) {
        return result;
    }

    while (query.next()) {
        ParameterSample sample;
        sample.aircraftTail = query.value(0).toString();
        sample.parameterName = query.value(1).toString();
        sample.value = query.value(2).toDouble();
        sample.timestamp = QDateTime::fromString(query.value(3).toString(), Qt::ISODateWithMs);
        result.push_back(sample);
    }
    return result;
}

} // namespace qttutorial::avionics
