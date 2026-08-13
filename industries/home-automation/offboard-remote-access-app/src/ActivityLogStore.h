// SPDX-License-Identifier: MIT
#pragma once

#include <QSqlDatabase>
#include <QString>

#include <vector>

namespace qttutorial::homeautomation::remote {

struct ActivityLogEntry {
    QString description;
    qint64 timestampMs = 0;
};

// SQLite-backed activity log of device-state changes observed by the remote
// app, standing in for the kind of durable history a "control your home
// from your phone" app keeps so you can check what happened while you were
// away. Independent of any UI so it can be unit tested against an
// in-memory database.
class ActivityLogStore {
public:
    explicit ActivityLogStore(const QString& databasePath, const QString& connectionName);
    ~ActivityLogStore();

    ActivityLogStore(const ActivityLogStore&) = delete;
    ActivityLogStore& operator=(const ActivityLogStore&) = delete;

    [[nodiscard]] bool isOpen() const;
    bool record(const QString& description, qint64 timestampMs);
    // Most-recent-first, capped at `limit` entries.
    [[nodiscard]] std::vector<ActivityLogEntry> recentEntries(int limit) const;
    [[nodiscard]] int entryCount() const;

private:
    QSqlDatabase m_db;
    QString m_connectionName;
};

} // namespace qttutorial::homeautomation::remote
