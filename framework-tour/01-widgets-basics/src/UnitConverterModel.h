// SPDX-License-Identifier: MIT
#pragma once

#include "UnitConversion.h"

#include <QAbstractTableModel>

#include <vector>

namespace qttutorial::widgets_basics {

// A small, real QAbstractTableModel: each row is one conversion request, editable
// in-place, recomputed live. Demonstrates Model/View plus C++23's std::expected used
// all the way from the domain layer up to model data().
class UnitConverterModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column { ValueColumn, FromColumn, ToColumn, ResultColumn, ColumnCount };

    struct Row {
        double value = 1.0;
        QString from;
        QString to;
    };

    explicit UnitConverterModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void addRow(Row row);
    void removeRow(int row);

private:
    std::vector<Row> m_rows;
};

} // namespace qttutorial::widgets_basics
