// SPDX-License-Identifier: MIT
#pragma once

#include <QDateTime>
#include <QSqlDatabase>
#include <QString>

#include <optional>
#include <vector>

namespace qttutorial::sql_persistence {

struct Task {
    int id = -1;
    QString title;
    bool done = false;
    QDateTime createdAt;
    int priority = 0;
};

// Owns a QSqlDatabase connection and evolves its schema through an ordered list
// of migrations, tracked in a one-row "schema_version" table. Every migration is
// a small, additive step (create table / add column) applied exactly once, in
// order, so the same repository code works whether it opens a brand-new
// database or one left behind by an earlier version of this program.
//
// All CRUD operations use QSqlQuery::prepare()/bindValue() placeholders; no SQL
// string is ever built by concatenating user-supplied values, which is what
// keeps this code free of SQL-injection risk even though the demo only ever
// talks to a private in-memory database.
class TaskRepository {
public:
    // connectionName must be unique per QSqlDatabase connection (Qt keys
    // connections by name), which lets tests open independent in-memory
    // databases without interfering with each other or with the app.
    explicit TaskRepository(const QString& connectionName, const QString& databaseName = QStringLiteral(":memory:"));
    ~TaskRepository();

    TaskRepository(const TaskRepository&) = delete;
    TaskRepository& operator=(const TaskRepository&) = delete;

    [[nodiscard]] bool isOpen() const;
    [[nodiscard]] QSqlDatabase database() const;

    // Creates schema_version if missing and applies every migration whose
    // number is greater than the version currently stored. Safe to call
    // repeatedly; a no-op once the schema is up to date.
    [[nodiscard]] bool applyMigrations();
    [[nodiscard]] int schemaVersion() const;

    [[nodiscard]] bool addTask(const QString& title, int priority = 0);
    [[nodiscard]] bool updateTask(const Task& task);
    [[nodiscard]] bool setDone(int id, bool done);
    [[nodiscard]] bool removeTask(int id);
    [[nodiscard]] std::vector<Task> allTasks() const;
    [[nodiscard]] std::optional<Task> findTask(int id) const;

private:
    [[nodiscard]] bool ensureSchemaVersionTable();
    [[nodiscard]] bool runMigration1();
    [[nodiscard]] bool runMigration2();
    void setSchemaVersion(int version);

    QSqlDatabase m_db;
    QString m_connectionName;
};

} // namespace qttutorial::sql_persistence
