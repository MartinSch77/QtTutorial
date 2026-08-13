// SPDX-License-Identifier: GPL-3.0-or-later
#include "TickerListModel.h"

#include <algorithm>

namespace qttutorial::stock_tracker {

TickerListModel::TickerListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int TickerListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_entries.size());
}

QVariant TickerListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_entries.size())) {
        return {};
    }
    const Entry& entry = m_entries[static_cast<std::size_t>(index.row())];
    const double change = entry.price - entry.openPrice;
    switch (role) {
    case SymbolRole:
        return entry.symbol;
    case PriceRole:
        return entry.price;
    case ChangeRole:
        return change;
    case ChangePercentRole:
        return entry.openPrice != 0.0 ? (change / entry.openPrice) * 100.0 : 0.0;
    case UpRole:
        return change >= 0.0;
    default:
        return {};
    }
}

QHash<int, QByteArray> TickerListModel::roleNames() const
{
    return {
        {SymbolRole, "symbol"},
        {PriceRole, "price"},
        {ChangeRole, "change"},
        {ChangePercentRole, "changePercent"},
        {UpRole, "up"},
    };
}

void TickerListModel::addSymbol(const QString& symbol, double openPrice)
{
    const int newIndex = static_cast<int>(m_entries.size());
    beginInsertRows({}, newIndex, newIndex);
    m_entries.push_back({symbol, openPrice, openPrice});
    endInsertRows();
}

void TickerListModel::updatePrice(const QString& symbol, double price)
{
    const int row = indexOfSymbol(symbol);
    if (row < 0) {
        return;
    }
    m_entries[static_cast<std::size_t>(row)].price = price;
    const QModelIndex modelIndex = index(row);
    emit dataChanged(modelIndex, modelIndex);
}

int TickerListModel::indexOfSymbol(const QString& symbol) const
{
    const auto it = std::find_if(m_entries.begin(), m_entries.end(),
                                  [&](const Entry& entry) { return entry.symbol == symbol; });
    return it == m_entries.end() ? -1 : static_cast<int>(std::distance(m_entries.begin(), it));
}

} // namespace qttutorial::stock_tracker
