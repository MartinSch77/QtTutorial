// SPDX-License-Identifier: MIT
#pragma once

#include "FleetSimulator.h"

#include <QAbstractTableModel>

#include <vector>

namespace qttutorial::fleet_ops {

// The live fleet table: one row per train, replaced wholesale each tick
// (cheap for the handful of rows a fleet dashboard shows, unlike the
// in-place row updates used in the SCADA tag table for a fixed row set).
class FleetTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column { TrainIdColumn, PositionColumn, SpeedColumn, DelayColumn, NextStopColumn, StatusColumn, ColumnCount };

    explicit FleetTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setStates(std::vector<TrainState> states);
    [[nodiscard]] const std::vector<TrainState>& states() const { return m_states; }

private:
    std::vector<TrainState> m_states;
};

} // namespace qttutorial::fleet_ops
