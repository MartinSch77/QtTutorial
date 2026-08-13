// SPDX-License-Identifier: MIT
#pragma once

#include "AlarmEvaluator.h"
#include "ProcessPointSimulator.h"

#include <QAbstractTableModel>

#include <vector>

namespace qttutorial::plant_scada {

// The live tag list shown in the SCADA table: one row per process point,
// updated in place as fresh samples arrive rather than rebuilt from scratch.
class TagTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column { TagIdColumn, ValueColumn, UnitColumn, StatusColumn, ColumnCount };

    explicit TagTableModel(std::vector<TagDefinition> tags, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void updateValue(int row, double value);
    [[nodiscard]] const TagDefinition& tagAt(int row) const { return m_tags.at(static_cast<std::size_t>(row)); }
    [[nodiscard]] int rowCountFor() const { return static_cast<int>(m_tags.size()); }

private:
    std::vector<TagDefinition> m_tags;
    std::vector<double> m_latestValues;
};

} // namespace qttutorial::plant_scada
