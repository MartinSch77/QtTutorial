// SPDX-License-Identifier: MIT
#include "UnitConverterModel.h"

namespace qttutorial::widgets_basics {

UnitConverterModel::UnitConverterModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    m_rows.push_back({100.0, "km", "mi"});
    m_rows.push_back({0.0, "C", "F"});
    m_rows.push_back({1.0, "kg", "lb"});
}

int UnitConverterModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_rows.size());
}

int UnitConverterModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant UnitConverterModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_rows.size())) {
        return {};
    }
    const Row& row = m_rows[static_cast<std::size_t>(index.row())];

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case ValueColumn:
            return row.value;
        case FromColumn:
            return row.from;
        case ToColumn:
            return row.to;
        case ResultColumn: {
            const auto result = units::convert(row.value, row.from.toStdString(), row.to.toStdString());
            if (!result) {
                switch (result.error()) {
                case units::ConversionError::UnknownUnit:
                    return QStringLiteral("unknown unit");
                case units::ConversionError::IncompatibleQuantities:
                    return QStringLiteral("incompatible units");
                }
            }
            return *result;
        }
        default:
            return {};
        }
    }
    return {};
}

bool UnitConverterModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole || !index.isValid() || index.row() >= static_cast<int>(m_rows.size())) {
        return false;
    }
    Row& row = m_rows[static_cast<std::size_t>(index.row())];
    switch (index.column()) {
    case ValueColumn:
        row.value = value.toDouble();
        break;
    case FromColumn:
        row.from = value.toString();
        break;
    case ToColumn:
        row.to = value.toString();
        break;
    default:
        return false;
    }
    emit dataChanged(index, index.siblingAtColumn(ColumnCount - 1));
    return true;
}

Qt::ItemFlags UnitConverterModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    if (index.column() == ResultColumn) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant UnitConverterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    switch (section) {
    case ValueColumn:
        return QStringLiteral("Value");
    case FromColumn:
        return QStringLiteral("From");
    case ToColumn:
        return QStringLiteral("To");
    case ResultColumn:
        return QStringLiteral("Result");
    default:
        return {};
    }
}

void UnitConverterModel::addRow(Row row)
{
    const int newIndex = static_cast<int>(m_rows.size());
    beginInsertRows({}, newIndex, newIndex);
    m_rows.push_back(std::move(row));
    endInsertRows();
}

void UnitConverterModel::removeRow(int row)
{
    if (row < 0 || row >= static_cast<int>(m_rows.size())) {
        return;
    }
    beginRemoveRows({}, row, row);
    m_rows.erase(m_rows.begin() + row);
    endRemoveRows();
}

} // namespace qttutorial::widgets_basics
