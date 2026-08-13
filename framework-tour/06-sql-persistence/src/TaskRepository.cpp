// SPDX-License-Identifier: MIT
#include "TaskRepository.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

namespace qttutorial::sql_persistence {

namespace {
constexpr int kLatestSchemaVersion = 2;
}

TaskRepository::TaskRepository(const QString& connectionName, const QString& databaseName)
    : m_db(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName))
    , m_connectionName(connectionName)
{
    m_db.setDatabaseName(databaseName);
    if (!m_db.open()) {
        return;
    }
}

TaskRepository::~TaskRepository()
{
    m_db.close();
    // Drop this object's own reference to the named connection before removing
    // it; QSqlDatabase::removeDatabase() warns (harmlessly, but noisily) if any
    // QSqlDatabase handle -- including this one -- still refers to the
    // connection being removed.
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(m_connectionName);
}

bool TaskRepository::isOpen() const
{
    return m_db.isOpen();
}

QSqlDatabase TaskRepository::database() const
{
    return m_db;
}

bool TaskRepository::ensureSchemaVersionTable()
{
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS schema_version (version INTEGER NOT NULL)"))) {
        return false;
    }

    QSqlQuery countQuery(m_db);
    if (!countQuery.exec(QStringLiteral("SELECT COUNT(*) FROM schema_version")) || !countQuery.next()) {
        return false;
    }
    if (countQuery.value(0).toInt() == 0) {
        QSqlQuery insertQuery(m_db);
        if (!insertQuery.prepare(QStringLiteral("INSERT INTO schema_version (version) VALUES (:version)"))) {
            return false;
        }
        insertQuery.bindValue(QStringLiteral(":version"), 0);
        if (!insertQuery.exec()) {
            return false;
        }
    }
    return true;
}

int TaskRepository::schemaVersion() const
{
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral("SELECT version FROM schema_version")) || !query.next()) {
        return 0;
    }
    return query.value(0).toInt();
}

void TaskRepository::setSchemaVersion(int version)
{
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("UPDATE schema_version SET version = :version"))) {
        return;
    }
    query.bindValue(QStringLiteral(":version"), version);
    if (!query.exec()) {
        return;
    }
}

bool TaskRepository::runMigration1()
{
    // Initial schema: one row per task.
    QSqlQuery query(m_db);
    return query.exec(QStringLiteral(
        "CREATE TABLE tasks ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  title TEXT NOT NULL,"
        "  done INTEGER NOT NULL DEFAULT 0,"
        "  created_at TEXT NOT NULL"
        ")"));
}

bool TaskRepository::runMigration2()
{
    // Additive change: a later release wants task priority, without breaking
    // rows created by migration 1. ALTER TABLE ... ADD COLUMN with a default
    // is the realistic, non-destructive way to evolve an existing table.
    QSqlQuery query(m_db);
    return query.exec(QStringLiteral("ALTER TABLE tasks ADD COLUMN priority INTEGER NOT NULL DEFAULT 0"));
}

bool TaskRepository::applyMigrations()
{
    if (!m_db.isOpen()) {
        return false;
    }
    if (!ensureSchemaVersionTable()) {
        return false;
    }

    int version = schemaVersion();

    if (version < 1) {
        if (!runMigration1()) {
            return false;
        }
        version = 1;
        setSchemaVersion(version);
    }
    if (version < 2) {
        if (!runMigration2()) {
            return false;
        }
        version = 2;
        setSchemaVersion(version);
    }

    return version == kLatestSchemaVersion;
}

bool TaskRepository::addTask(const QString& title, int priority)
{
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral(
            "INSERT INTO tasks (title, done, created_at, priority) VALUES (:title, :done, :created_at, :priority)"))) {
        return false;
    }
    query.bindValue(QStringLiteral(":title"), title);
    query.bindValue(QStringLiteral(":done"), 0);
    query.bindValue(QStringLiteral(":created_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    query.bindValue(QStringLiteral(":priority"), priority);
    return query.exec();
}

bool TaskRepository::updateTask(const Task& task)
{
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral(
            "UPDATE tasks SET title = :title, done = :done, priority = :priority WHERE id = :id"))) {
        return false;
    }
    query.bindValue(QStringLiteral(":title"), task.title);
    query.bindValue(QStringLiteral(":done"), task.done ? 1 : 0);
    query.bindValue(QStringLiteral(":priority"), task.priority);
    query.bindValue(QStringLiteral(":id"), task.id);
    return query.exec() && query.numRowsAffected() > 0;
}

bool TaskRepository::setDone(int id, bool done)
{
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("UPDATE tasks SET done = :done WHERE id = :id"))) {
        return false;
    }
    query.bindValue(QStringLiteral(":done"), done ? 1 : 0);
    query.bindValue(QStringLiteral(":id"), id);
    return query.exec() && query.numRowsAffected() > 0;
}

bool TaskRepository::removeTask(int id)
{
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral("DELETE FROM tasks WHERE id = :id"))) {
        return false;
    }
    query.bindValue(QStringLiteral(":id"), id);
    return query.exec() && query.numRowsAffected() > 0;
}

std::vector<Task> TaskRepository::allTasks() const
{
    std::vector<Task> tasks;
    QSqlQuery query(m_db);
    if (!query.exec(QStringLiteral(
            "SELECT id, title, done, created_at, priority FROM tasks ORDER BY id ASC"))) {
        return tasks;
    }
    while (query.next()) {
        Task task;
        task.id = query.value(0).toInt();
        task.title = query.value(1).toString();
        task.done = query.value(2).toInt() != 0;
        task.createdAt = QDateTime::fromString(query.value(3).toString(), Qt::ISODate);
        task.priority = query.value(4).toInt();
        tasks.push_back(task);
    }
    return tasks;
}

std::optional<Task> TaskRepository::findTask(int id) const
{
    QSqlQuery query(m_db);
    if (!query.prepare(QStringLiteral(
            "SELECT id, title, done, created_at, priority FROM tasks WHERE id = :id"))) {
        return std::nullopt;
    }
    query.bindValue(QStringLiteral(":id"), id);
    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }
    Task task;
    task.id = query.value(0).toInt();
    task.title = query.value(1).toString();
    task.done = query.value(2).toInt() != 0;
    task.createdAt = QDateTime::fromString(query.value(3).toString(), Qt::ISODate);
    task.priority = query.value(4).toInt();
    return task;
}

} // namespace qttutorial::sql_persistence
