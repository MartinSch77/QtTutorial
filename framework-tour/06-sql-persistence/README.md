# 06 – SQL Persistence

A small task list backed by SQLite through Qt SQL, demonstrating three things
that come up in almost every real Qt application that touches a database:

1. A repository class that hides SQL behind a typed C++ API.
2. A versioned migration mechanism so the schema can evolve across releases.
3. Parameterized queries everywhere, so no SQL is ever built by string
   concatenation.

## The repository pattern

`TaskRepository` (`src/TaskRepository.h/.cpp`) owns one `QSqlDatabase`
connection and exposes the operations the rest of the app actually needs —
`addTask`, `updateTask`, `setDone`, `removeTask`, `allTasks`, `findTask` —
instead of letting callers write ad hoc SQL. This keeps the SQL in one place,
makes it easy to unit test without a UI, and means a future change (say,
swapping SQLite for another Qt SQL driver) only touches this one class.

The constructor takes a **connection name**, not just a database name,
because `QSqlDatabase::addDatabase()` keys connections globally by name. Two
`TaskRepository` instances with different connection names, even both backed
by `:memory:` SQLite, get two completely independent, private databases. The
unit tests rely on this to run several database-touching tests side by side
without interfering with each other.

## Why parameterized queries

Every query in `TaskRepository.cpp` is built with `QSqlQuery::prepare()` and
`bindValue()`:

```cpp
query.prepare(QStringLiteral("INSERT INTO tasks (title, done, created_at, priority) "
                              "VALUES (:title, :done, :created_at, :priority)"));
query.bindValue(QStringLiteral(":title"), title);
```

No query is ever assembled by concatenating a title, id, or other
caller-supplied value directly into the SQL text. That would let a value like
`Robert'); DROP TABLE tasks;--` change the meaning of the statement — the
classic SQL injection. This demo only ever talks to a private in-memory
database, so there's no attacker here, but the same repository code would be
safe to point at a real, shared database, because bound parameters are always
sent as data, never as SQL syntax. This is worth building as muscle memory:
write the parameterized version even for "safe" internal tools, because code
tends to get reused in less safe contexts.

## The versioned migration mechanism

Schema changes are modeled as an ordered list of migrations, tracked by a
single-row `schema_version` table:

- `ensureSchemaVersionTable()` creates `schema_version` if it doesn't exist
  yet and seeds it with `version = 0` for a brand-new database.
- `applyMigrations()` reads the current version and applies every migration
  whose number is greater than it, in order, bumping the stored version after
  each one succeeds.
- Migration 1 creates the initial `tasks` table (`id`, `title`, `done`,
  `created_at`).
- Migration 2 is a realistic *additive* evolution: it adds a `priority`
  column to the existing table with `ALTER TABLE tasks ADD COLUMN priority
  INTEGER NOT NULL DEFAULT 0`, rather than recreating the table. Existing
  rows keep their data and simply get `priority = 0`.

Because `applyMigrations()` only ever runs the migrations that are still
missing, it is safe to call on every startup: a fresh database walks through
migrations 1 and 2; a database left behind by an older build of this program
(already at version 1) only runs migration 2; a database already at version 2
does nothing.

### Adding migration 3

To add another schema change:

1. Add a `runMigration3()` private method that performs the change (e.g.
   `CREATE TABLE tags (...)` or another `ALTER TABLE`).
2. Bump `kLatestSchemaVersion` in `TaskRepository.cpp` to `3`.
3. In `applyMigrations()`, add an `if (version < 3) { ... }` block that calls
   `runMigration3()` and then `setSchemaVersion(3)`, following the same
   pattern used for migrations 1 and 2.
4. Extend the CRUD methods (and the `Task` struct, if the new column belongs
   on it) to read/write the new column — still exclusively through
   `prepare()`/`bindValue()`.
5. Add a test asserting `schemaVersion() == 3` after `applyMigrations()`, plus
   coverage for whatever new behavior the migration enables.

## The UI

`MainWindow` uses `QSqlTableModel` bound directly to the `tasks` table on the
same `QSqlDatabase` connection the repository opened. `QSqlTableModel` is used
with `OnManualSubmit`: edits are buffered until "Save changes" calls
`submitAll()`. Adding, removing, and toggling "done" go through
`TaskRepository` (so they stay covered by the same parameterized-query path
used in tests), followed by `m_model->select()` to refresh the view.
