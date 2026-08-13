# 06 – SQL Persistence

This module builds a small task list backed by SQLite through Qt SQL. It
covers three ideas that show up in most real Qt apps that touch a database:
wrapping SQL in a repository class, evolving a schema with versioned
migrations, and using parameterized queries so no SQL is ever built by string
concatenation.

The code lives in `framework-tour/06-sql-persistence/`, with a matching test
in `tests/framework-tour/06-sql-persistence/`.

## 1. Why a repository class

Instead of scattering `QSqlQuery` calls across the UI code, this module puts
every query behind a single class, `TaskRepository`
(`framework-tour/06-sql-persistence/src/TaskRepository.h`):

```cpp
class TaskRepository {
public:
    explicit TaskRepository(const QString& connectionName,
                             const QString& databaseName = QStringLiteral(":memory:"));

    [[nodiscard]] bool applyMigrations();
    [[nodiscard]] int schemaVersion() const;

    [[nodiscard]] bool addTask(const QString& title, int priority = 0);
    [[nodiscard]] bool updateTask(const Task& task);
    [[nodiscard]] bool setDone(int id, bool done);
    [[nodiscard]] bool removeTask(int id);
    [[nodiscard]] std::vector<Task> allTasks() const;
    [[nodiscard]] std::optional<Task> findTask(int id) const;
    // ...
};
```

The UI, and the unit tests, only ever call these methods — never raw SQL.
That keeps the SQL in one reviewable place and makes the logic testable
without spinning up a `QApplication` or a widget.

The constructor takes a **connection name** (in addition to a database
name). Qt's `QSqlDatabase::addDatabase()` keys connections by name in a
process-wide table, so two `TaskRepository` instances constructed with
different connection names — even if both use the special `:memory:` SQLite
database name — get two independent, private in-memory databases. Naming
connections explicitly is what lets several tests below open the database
"fresh" without any cross-test leakage.

## 2. Opening the in-memory database

```cpp
m_db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName);
m_db.setDatabaseName(databaseName); // ":memory:" by default
m_db.open();
```

`QSQLITE` is bundled with Qt, so no extra driver installation is required.
`:memory:` gives a database that lives only for the lifetime of that
connection and is never written to disk — convenient for a tutorial and for
tests, since each run starts from a guaranteed-empty schema.

## 3. A real, versioned migration mechanism

A schema rarely stays fixed for the life of an application. This module
tracks the schema's version in a one-row `schema_version` table and applies
whichever migrations haven't run yet:

```cpp
bool TaskRepository::applyMigrations()
{
    if (!m_db.isOpen()) return false;
    if (!ensureSchemaVersionTable()) return false;

    int version = schemaVersion();
    if (version < 1) { if (!runMigration1()) return false; version = 1; setSchemaVersion(version); }
    if (version < 2) { if (!runMigration2()) return false; version = 2; setSchemaVersion(version); }
    return version == kLatestSchemaVersion;
}
```

- **Migration 1** creates the initial table:

  ```sql
  CREATE TABLE tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    done INTEGER NOT NULL DEFAULT 0,
    created_at TEXT NOT NULL
  )
  ```

- **Migration 2** demonstrates a realistic, *additive* schema change rather
  than a second unrelated `CREATE TABLE`: it adds a `priority` column to the
  table created by migration 1, without touching existing rows:

  ```sql
  ALTER TABLE tasks ADD COLUMN priority INTEGER NOT NULL DEFAULT 0
  ```

Because `applyMigrations()` re-reads the stored version and only runs the
migrations still missing, calling it on every startup is safe and idempotent:
a brand-new database runs both migrations, a database already at version 1
(from an older build) only runs migration 2, and a database already at
version 2 does nothing at all.

**Adding migration 3** later would mean: write `runMigration3()`, bump
`kLatestSchemaVersion` to `3`, add an `if (version < 3) { ... }` block in
`applyMigrations()` mirroring migrations 1 and 2, and extend the `Task`
struct/CRUD methods and tests for whatever the new migration adds. See the
module's `README.md` for the full checklist.

## 4. Parameterized queries — no string-built SQL

Every query in `TaskRepository.cpp` uses placeholders:

```cpp
query.prepare(QStringLiteral(
    "INSERT INTO tasks (title, done, created_at, priority) "
    "VALUES (:title, :done, :created_at, :priority)"));
query.bindValue(QStringLiteral(":title"), title);
query.bindValue(QStringLiteral(":done"), 0);
query.bindValue(QStringLiteral(":created_at"), QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
query.bindValue(QStringLiteral(":priority"), priority);
query.exec();
```

No caller-supplied value (a task title, an id) is ever spliced directly into
a SQL string. If it were, a title containing something like
`'); DROP TABLE tasks;--` could change what the database executes — a SQL
injection. Binding values through `bindValue()` sends them purely as data,
never as SQL syntax, so this risk doesn't exist regardless of what a title
contains. This module only ever talks to a private in-memory database, but
writing every query this way, even for "internal" or "demo" code, is what
keeps the habit from failing you the day the same code starts talking to a
real, shared database.

## 5. The Widgets UI

`MainWindow` (`framework-tour/06-sql-persistence/src/MainWindow.h/.cpp`) binds
a `QSqlTableModel` directly to the `tasks` table, over the *same*
`QSqlDatabase` connection the repository opened:

```cpp
m_repository = std::make_unique<TaskRepository>(QStringLiteral("sql_persistence_app"));
m_repository->applyMigrations();

m_model = new QSqlTableModel(this, m_repository->database());
m_model->setTable(QStringLiteral("tasks"));
m_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
m_model->select();
```

`OnManualSubmit` buffers edits until a "Save changes" button calls
`m_model->submitAll()`. Add/remove/toggle-done buttons call into
`TaskRepository` (so they run through the same parameterized-query path
tested below), then call `m_model->select()` to refresh the view from the
database.

## 6. Testing the repository

`tests/framework-tour/06-sql-persistence/test_sql_persistence.cpp` is a QTest
class that:

- Opens a uniquely-named in-memory connection per test method (e.g.
  `test_sql_persistence_crud`), so tests never see each other's data.
- Calls `applyMigrations()` and asserts `schemaVersion() == 2`, both on a
  fresh database and after calling it a second time (idempotency).
- Exercises the full CRUD round trip — add, read back, update, toggle
  `done`, remove, confirm it's gone — entirely through `TaskRepository`'s
  typed API, never through raw SQL in the test itself.
- Confirms two repositories with different connection names don't share
  rows, verifying the connection-name isolation described above.

Build and run it the same way as the other framework-tour modules' tests,
via `ctest` from the build directory (see the top-level tutorial README for
the full build instructions).
