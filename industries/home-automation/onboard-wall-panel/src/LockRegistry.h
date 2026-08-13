// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

#include <vector>

namespace qttutorial::homeautomation {

struct Lock {
    QString name;
    bool locked = true;
};

// One entry in the "last changed by" audit log kept alongside the locks -
// standing in for the kind of local access log a real smart lock keeps.
struct LockLogEntry {
    QString lockName;
    bool locked = true;
    QString actor;
    qint64 timestampMs = 0;
};

// Holds door lock state plus a change log. No Qt GUI dependency, so it is
// unit testable independent of the QML wall panel.
class LockRegistry {
public:
    explicit LockRegistry(std::vector<Lock> locks = defaultLocks());

    [[nodiscard]] int count() const { return static_cast<int>(m_locks.size()); }
    [[nodiscard]] const Lock& lock(int index) const { return m_locks.at(static_cast<std::size_t>(index)); }

    // No-op (and does not log) if the lock is already in the requested
    // state, so the log only records real changes.
    void setLocked(int index, bool locked, const QString& actor, qint64 timestampMs);

    [[nodiscard]] const std::vector<LockLogEntry>& log() const { return m_log; }
    // Most-recent-first, capped at `limit` entries.
    [[nodiscard]] std::vector<LockLogEntry> recentLog(int limit) const;

    [[nodiscard]] static std::vector<Lock> defaultLocks();

private:
    [[nodiscard]] bool isValidIndex(int index) const;

    std::vector<Lock> m_locks;
    std::vector<LockLogEntry> m_log;
};

} // namespace qttutorial::homeautomation
