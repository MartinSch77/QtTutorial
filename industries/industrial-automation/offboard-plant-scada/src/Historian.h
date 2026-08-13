// SPDX-License-Identifier: MIT
#pragma once

#include <QDateTime>
#include <QSqlDatabase>
#include <QString>

#include <optional>
#include <vector>

namespace qttutorial::plant_scada {

struct Sample {
    QDateTime timestamp;
    double value = 0.0;
};

// A minimal process historian on top of SQLite: a real schema (tag_id,
// timestamp, value), parameterized statements throughout, and range queries
// so trends can be pulled back out for a given tag and time window. Each
// instance opens its own uniquely named QSqlDatabase connection so several
// can coexist (e.g. one per test).
class Historian {
public:
    explicit Historian(const QString& connectionName, const QString& databasePath = QStringLiteral(":memory:"));
    ~Historian();

    Historian(const Historian&) = delete;
    Historian& operator=(const Historian&) = delete;

    [[nodiscard]] bool isOpen() const;
    bool createSchema();

    bool insertSample(const QString& tagId, const QDateTime& timestamp, double value);

    [[nodiscard]] std::vector<Sample> samplesInRange(const QString& tagId, const QDateTime& from,
                                                      const QDateTime& to) const;

    [[nodiscard]] std::optional<Sample> latestSample(const QString& tagId) const;

private:
    QString m_connectionName;
};

} // namespace qttutorial::plant_scada
