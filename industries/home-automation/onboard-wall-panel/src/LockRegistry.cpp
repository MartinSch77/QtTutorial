// SPDX-License-Identifier: MIT
#include "LockRegistry.h"

#include <algorithm>

namespace qttutorial::homeautomation {

LockRegistry::LockRegistry(std::vector<Lock> locks)
    : m_locks(std::move(locks))
{
}

bool LockRegistry::isValidIndex(int index) const
{
    return index >= 0 && index < count();
}

void LockRegistry::setLocked(int index, bool locked, const QString& actor, qint64 timestampMs)
{
    if (!isValidIndex(index)) {
        return;
    }
    Lock& target = m_locks[static_cast<std::size_t>(index)];
    if (target.locked == locked) {
        return;
    }
    target.locked = locked;
    m_log.push_back(LockLogEntry{.lockName = target.name, .locked = locked, .actor = actor, .timestampMs = timestampMs});
}

std::vector<LockLogEntry> LockRegistry::recentLog(int limit) const
{
    const int take = std::min<int>(limit, static_cast<int>(m_log.size()));
    std::vector<LockLogEntry> result(m_log.end() - take, m_log.end());
    std::reverse(result.begin(), result.end());
    return result;
}

std::vector<Lock> LockRegistry::defaultLocks()
{
    return {
        Lock{.name = QStringLiteral("Front Door"), .locked = true},
        Lock{.name = QStringLiteral("Back Door"), .locked = true},
        Lock{.name = QStringLiteral("Garage Door"), .locked = false},
    };
}

} // namespace qttutorial::homeautomation
