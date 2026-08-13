// SPDX-License-Identifier: MIT
#include "AssetTableModel.h"

#include <QColor>

namespace qttutorial::defence {

AssetTableModel::AssetTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int AssetTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_assets.size());
}

int AssetTableModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant AssetTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_assets.size())) {
        return {};
    }
    const Asset& asset = m_assets[static_cast<std::size_t>(index.row())];

    if (role == Qt::BackgroundRole) {
        return backgroundFor(asset, index.column());
    }
    if (role == Qt::DisplayRole) {
        return displayValueFor(asset, index.column());
    }
    return {};
}

QVariant AssetTableModel::backgroundFor(const Asset& asset, int column)
{
    if (column == HealthColumn) {
        if (asset.health == QStringLiteral("Critical")) {
            return QColor(0xa0, 0x20, 0x20);
        }
        if (asset.health == QStringLiteral("Caution")) {
            return QColor(0xb8, 0x86, 0x0b);
        }
        return QColor(0x2a, 0x8a, 0x2a);
    }
    if (column == LinkColumn && asset.trackStale) {
        return QColor(0x5a, 0x46, 0x14);
    }
    return {};
}

QVariant AssetTableModel::displayValueFor(const Asset& asset, int column)
{
    switch (column) {
    case IdColumn:
        return asset.id;
    case TypeColumn:
        return toString(asset.type);
    case HealthColumn:
        return asset.health;
    case LinkColumn:
        return asset.trackStale
                ? QStringLiteral("STALE (%1s)").arg(QString::number(asset.dataAgeSeconds, 'f', 0))
                : QStringLiteral("Current (%1%)").arg(QString::number(asset.commsQualityPercent, 'f', 0));
    case PositionColumn:
        return QStringLiteral("%1, %2 km").arg(QString::number(asset.xKm, 'f', 1),
                                                QString::number(asset.yKm, 'f', 1));
    default:
        return {};
    }
}

QVariant AssetTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    switch (section) {
    case IdColumn:
        return QStringLiteral("Asset");
    case TypeColumn:
        return QStringLiteral("Type");
    case HealthColumn:
        return QStringLiteral("Health");
    case LinkColumn:
        return QStringLiteral("Data Link");
    case PositionColumn:
        return QStringLiteral("Last-Known Position");
    default:
        return {};
    }
}

void AssetTableModel::setAssets(std::vector<Asset> assets)
{
    beginResetModel();
    m_assets = std::move(assets);
    endResetModel();
}

const Asset* AssetTableModel::assetAt(int row) const
{
    if (row < 0 || row >= static_cast<int>(m_assets.size())) {
        return nullptr;
    }
    return &m_assets[static_cast<std::size_t>(row)];
}

} // namespace qttutorial::defence
