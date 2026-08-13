// SPDX-License-Identifier: MIT
#include "LockListModel.h"

#include <QDateTime>

namespace qttutorial::homeautomation {

namespace {
constexpr int kRecentActivityLimit = 6;
}

LockListModel::LockListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int LockListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_registry.count();
}

QVariant LockListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_registry.count()) {
        return {};
    }
    const Lock& lock = m_registry.lock(index.row());
    switch (role) {
    case NameRole:
        return lock.name;
    case LockedRole:
        return lock.locked;
    default:
        return {};
    }
}

QHash<int, QByteArray> LockListModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {LockedRole, "locked"},
    };
}

void LockListModel::setLocked(int row, bool locked)
{
    m_registry.setLocked(row, locked, QStringLiteral("Wall Panel"), QDateTime::currentMSecsSinceEpoch());
    const QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {LockedRole});
    emit recentActivityChanged();
}

QStringList LockListModel::recentActivity() const
{
    QStringList lines;
    for (const LockLogEntry& entry : m_registry.recentLog(kRecentActivityLimit)) {
        const QString time = QDateTime::fromMSecsSinceEpoch(entry.timestampMs).toString(QStringLiteral("hh:mm:ss"));
        lines << QStringLiteral("%1 %2 by %3 at %4")
                     .arg(entry.lockName, entry.locked ? QStringLiteral("locked") : QStringLiteral("unlocked"), entry.actor, time);
    }
    return lines;
}

} // namespace qttutorial::homeautomation
