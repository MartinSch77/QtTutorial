// SPDX-License-Identifier: MIT
#include "VitalsHistoryStore.h"

#include <QSqlQuery>

#include <algorithm>

namespace qttutorial::medical::telehealth {

VitalsHistoryStore::VitalsHistoryStore(const QString& databasePath, const QString& connectionName)
    : m_db(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName))
    , m_connectionName(connectionName)
{
    m_db.setDatabaseName(databasePath);
    if (!m_db.open()) {
        return;
    }

    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS vitals_history ("
            "patient_id TEXT NOT NULL,"
            "timestamp_ms INTEGER NOT NULL,"
            "heart_rate REAL NOT NULL,"
            "spo2 REAL NOT NULL,"
            "systolic REAL NOT NULL,"
            "diastolic REAL NOT NULL"
            ")"))) {
        m_db.close();
    }
}

VitalsHistoryStore::~VitalsHistoryStore()
{
    m_db.close();
    QSqlDatabase::removeDatabase(m_connectionName);
}

bool VitalsHistoryStore::isOpen() const
{
    return m_db.isOpen();
}

bool VitalsHistoryStore::recordSample(const QString& patientId, qint64 timestampMs, double heartRate,
                                       double spo2, double systolic, double diastolic)
{
    if (!m_db.isOpen()) {
        return false;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral(
            "INSERT INTO vitals_history (patient_id, timestamp_ms, heart_rate, spo2, systolic, diastolic) "
            "VALUES (?, ?, ?, ?, ?, ?)"))) {
        return false;
    }
    query.addBindValue(patientId);
    query.addBindValue(timestampMs);
    query.addBindValue(heartRate);
    query.addBindValue(spo2);
    query.addBindValue(systolic);
    query.addBindValue(diastolic);
    return query.exec();
}

std::vector<double> VitalsHistoryStore::recentHeartRates(const QString& patientId, int limit) const
{
    std::vector<double> rates;
    if (!m_db.isOpen()) {
        return rates;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral(
            "SELECT heart_rate FROM vitals_history WHERE patient_id = ? "
            "ORDER BY timestamp_ms DESC LIMIT ?"))) {
        return rates;
    }
    query.addBindValue(patientId);
    query.addBindValue(limit);
    if (!query.exec()) {
        return rates;
    }
    while (query.next()) {
        rates.push_back(query.value(0).toDouble());
    }
    std::reverse(rates.begin(), rates.end());
    return rates;
}

int VitalsHistoryStore::sampleCount(const QString& patientId) const
{
    if (!m_db.isOpen()) {
        return 0;
    }
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("SELECT COUNT(*) FROM vitals_history WHERE patient_id = ?"))) {
        return 0;
    }
    query.addBindValue(patientId);
    if (!query.exec() || !query.next()) {
        return 0;
    }
    return query.value(0).toInt();
}

} // namespace qttutorial::medical::telehealth
