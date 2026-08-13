// SPDX-License-Identifier: MIT
#pragma once

#include "Asset.h"

#include <QAbstractTableModel>

#include <vector>

namespace qttutorial::defence {

class AssetTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column { IdColumn, TypeColumn, HealthColumn, PositionColumn, ColumnCount };

    explicit AssetTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setAssets(std::vector<Asset> assets);
    [[nodiscard]] const Asset* assetAt(int row) const;

private:
    std::vector<Asset> m_assets;
};

} // namespace qttutorial::defence
