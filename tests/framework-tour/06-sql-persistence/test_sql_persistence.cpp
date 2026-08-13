// SPDX-License-Identifier: MIT
#include "TaskRepository.h"

#include <QTest>

using namespace qttutorial::sql_persistence;

class TestSqlPersistence : public QObject {
    Q_OBJECT
private slots:
    void migrationsReachLatestVersion()
    {
        TaskRepository repo(QStringLiteral("test_sql_persistence_migrations"));
        QVERIFY(repo.isOpen());
        QVERIFY(repo.applyMigrations());
        QCOMPARE(repo.schemaVersion(), 2);
    }

    void migrationsAreIdempotent()
    {
        TaskRepository repo(QStringLiteral("test_sql_persistence_idempotent"));
        QVERIFY(repo.applyMigrations());
        QVERIFY(repo.applyMigrations());
        QCOMPARE(repo.schemaVersion(), 2);
    }

    void crudRoundTrip()
    {
        TaskRepository repo(QStringLiteral("test_sql_persistence_crud"));
        QVERIFY(repo.applyMigrations());

        QVERIFY(repo.addTask(QStringLiteral("Write tutorial"), 3));
        const auto tasksAfterAdd = repo.allTasks();
        QCOMPARE(tasksAfterAdd.size(), std::size_t(1));
        QCOMPARE(tasksAfterAdd[0].title, QStringLiteral("Write tutorial"));
        QCOMPARE(tasksAfterAdd[0].priority, 3);
        QVERIFY(!tasksAfterAdd[0].done);

        const int id = tasksAfterAdd[0].id;
        QVERIFY(id > 0);

        Task updated = tasksAfterAdd[0];
        updated.title = QStringLiteral("Write tutorial (revised)");
        updated.done = true;
        updated.priority = 5;
        QVERIFY(repo.updateTask(updated));

        const auto found = repo.findTask(id);
        QVERIFY(found.has_value());
        QCOMPARE(found->title, QStringLiteral("Write tutorial (revised)"));
        QVERIFY(found->done);
        QCOMPARE(found->priority, 5);

        QVERIFY(repo.setDone(id, false));
        const auto afterToggle = repo.findTask(id);
        QVERIFY(afterToggle.has_value());
        QVERIFY(!afterToggle->done);

        QVERIFY(repo.removeTask(id));
        QVERIFY(!repo.findTask(id).has_value());
        QVERIFY(repo.allTasks().empty());
    }

    void isolatedConnectionsDoNotShareData()
    {
        TaskRepository repoA(QStringLiteral("test_sql_persistence_isolation_a"));
        TaskRepository repoB(QStringLiteral("test_sql_persistence_isolation_b"));
        QVERIFY(repoA.applyMigrations());
        QVERIFY(repoB.applyMigrations());

        QVERIFY(repoA.addTask(QStringLiteral("Only in A")));
        QCOMPARE(repoA.allTasks().size(), std::size_t(1));
        QCOMPARE(repoB.allTasks().size(), std::size_t(0));
    }
};

QTEST_MAIN(TestSqlPersistence)
#include "test_sql_persistence.moc"
