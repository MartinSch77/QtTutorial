// SPDX-License-Identifier: MIT
#pragma once

#include "Fleet.h"

#include <QAbstractTableModel>

#include <vector>

namespace qttutorial::avionics {

class FleetTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column { TailColumn, TypeColumn, VibrationColumn, EgtMarginColumn, OilPressureColumn, StatusColumn, ColumnCount };

    explicit FleetTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setAircraft(std::vector<Aircraft> aircraft);
    [[nodiscard]] const Aircraft* aircraftAt(int row) const;

private:
    std::vector<Aircraft> m_aircraft;
};

} // namespace qttutorial::avionics
