// SPDX-License-Identifier: MIT
#include "TagTableModel.h"

namespace qttutorial::plant_scada {

TagTableModel::TagTableModel(std::vector<TagDefinition> tags, QObject* parent)
    : QAbstractTableModel(parent)
    , m_tags(std::move(tags))
    , m_latestValues(m_tags.size(), 0.0)
{
}

int TagTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_tags.size());
}

int TagTableModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant TagTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_tags.size())) {
        return {};
    }
    if (role != Qt::DisplayRole) {
        return {};
    }
    const TagDefinition& tag = m_tags[static_cast<std::size_t>(index.row())];
    const double value = m_latestValues[static_cast<std::size_t>(index.row())];
    switch (index.column()) {
    case TagIdColumn:
        return tag.tagId;
    case ValueColumn:
        return QString::number(value, 'f', 2);
    case UnitColumn:
        return tag.unit;
    case StatusColumn:
        return QString::fromLatin1(severityLabel(evaluate(tag, value)));
    default:
        return {};
    }
}

QVariant TagTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    switch (section) {
    case TagIdColumn:
        return QStringLiteral("Tag");
    case ValueColumn:
        return QStringLiteral("Value");
    case UnitColumn:
        return QStringLiteral("Unit");
    case StatusColumn:
        return QStringLiteral("Status");
    default:
        return {};
    }
}

void TagTableModel::updateValue(int row, double value)
{
    if (row < 0 || row >= static_cast<int>(m_tags.size())) {
        return;
    }
    m_latestValues[static_cast<std::size_t>(row)] = value;
    const QModelIndex topLeft = index(row, ValueColumn);
    const QModelIndex bottomRight = index(row, StatusColumn);
    emit dataChanged(topLeft, bottomRight);
}

} // namespace qttutorial::plant_scada
