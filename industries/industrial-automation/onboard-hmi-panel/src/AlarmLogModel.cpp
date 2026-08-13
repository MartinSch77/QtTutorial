// SPDX-License-Identifier: MIT
#include "AlarmLogModel.h"

#include <algorithm>

namespace qttutorial::industrial_hmi {

AlarmLogModel::AlarmLogModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int AlarmLogModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_entries.size());
}

QVariant AlarmLogModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_entries.size())) {
        return {};
    }
    const Entry& entry = m_entries[static_cast<std::size_t>(index.row())];
    switch (role) {
    case MessageRole:
        return entry.message;
    case SeverityRole:
        return static_cast<int>(entry.severity);
    case TimestampRole:
        return entry.timestamp;
    case AcknowledgedRole:
        return entry.acknowledged;
    default:
        return {};
    }
}

QHash<int, QByteArray> AlarmLogModel::roleNames() const
{
    return {
        {MessageRole, "message"},
        {SeverityRole, "severity"},
        {TimestampRole, "timestamp"},
        {AcknowledgedRole, "acknowledged"},
    };
}

void AlarmLogModel::raise(const QString& message, Severity severity)
{
    beginInsertRows({}, 0, 0);
    m_entries.insert(m_entries.begin(), Entry{message, severity, QDateTime::currentDateTime(), false});
    endInsertRows();

    if (m_entries.size() > kMaxEntries) {
        const int lastRow = static_cast<int>(m_entries.size()) - 1;
        beginRemoveRows({}, lastRow, lastRow);
        m_entries.pop_back();
        endRemoveRows();
    }
    emit countChanged();
    emit unacknowledgedCountChanged();
}

int AlarmLogModel::unacknowledgedCount() const
{
    return static_cast<int>(std::count_if(m_entries.begin(), m_entries.end(),
                                           [](const Entry& entry) { return !entry.acknowledged; }));
}

void AlarmLogModel::acknowledge(int row)
{
    if (row < 0 || row >= static_cast<int>(m_entries.size())) {
        return;
    }
    Entry& entry = m_entries[static_cast<std::size_t>(row)];
    if (entry.acknowledged) {
        return;
    }
    entry.acknowledged = true;
    const QModelIndex changed = index(row, 0);
    emit dataChanged(changed, changed, {AcknowledgedRole});
    emit unacknowledgedCountChanged();
}

void AlarmLogModel::acknowledgeAll()
{
    bool anyChanged = false;
    for (Entry& entry : m_entries) {
        if (!entry.acknowledged) {
            entry.acknowledged = true;
            anyChanged = true;
        }
    }
    if (anyChanged) {
        if (!m_entries.empty()) {
            emit dataChanged(index(0, 0), index(static_cast<int>(m_entries.size()) - 1, 0), {AcknowledgedRole});
        }
        emit unacknowledgedCountChanged();
    }
}

} // namespace qttutorial::industrial_hmi
