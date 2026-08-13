// SPDX-License-Identifier: MIT
#include "EventLogModel.h"

namespace qttutorial::factory_machine_panel {

EventLogModel::EventLogModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int EventLogModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_entries.size());
}

QVariant EventLogModel::data(const QModelIndex& index, int role) const
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
    default:
        return {};
    }
}

QHash<int, QByteArray> EventLogModel::roleNames() const
{
    return {
        {MessageRole, "message"},
        {SeverityRole, "severity"},
        {TimestampRole, "timestamp"},
    };
}

void EventLogModel::record(const QString& message, Severity severity)
{
    beginInsertRows({}, 0, 0);
    m_entries.insert(m_entries.begin(), Entry{message, severity, QDateTime::currentDateTime()});
    endInsertRows();

    if (m_entries.size() > kMaxEntries) {
        const int lastRow = static_cast<int>(m_entries.size()) - 1;
        beginRemoveRows({}, lastRow, lastRow);
        m_entries.pop_back();
        endRemoveRows();
    }
    emit countChanged();
}

} // namespace qttutorial::factory_machine_panel
