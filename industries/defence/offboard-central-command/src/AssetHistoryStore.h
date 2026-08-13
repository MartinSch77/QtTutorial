// SPDX-License-Identifier: MIT
#pragma once

#include "Asset.h"

#include <QSqlDatabase>
#include <QString>

#include <vector>

namespace qttutorial::defence {

struct AssetHistoryRecord {
    QString assetId;
    AssetType type = AssetType::Vehicle;
    double xKm = 0.0;
    double yKm = 0.0;
    double headingDeg = 0.0;
    QString health;
    QDateTime timestamp;
};

// SQLite-backed asset status history via QtSql. Every statement is
// parameterized (prepare + bindValue) - no string-built SQL anywhere in
// this class.
class AssetHistoryStore {
public:
    explicit AssetHistoryStore(const QString& connectionName);
    ~AssetHistoryStore();

    AssetHistoryStore(const AssetHistoryStore&) = delete;
    AssetHistoryStore& operator=(const AssetHistoryStore&) = delete;

    [[nodiscard]] bool open(const QString& databasePath);
    [[nodiscard]] bool recordStatus(const AssetHistoryRecord& record);
    [[nodiscard]] std::vector<AssetHistoryRecord> history(const QString& assetId, int limit = 200) const;

private:
    QString m_connectionName;
};

} // namespace qttutorial::defence
