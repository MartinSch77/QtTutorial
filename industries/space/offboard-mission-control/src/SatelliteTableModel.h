// SPDX-License-Identifier: MIT
#pragma once

#include "Satellite.h"

#include <QAbstractTableModel>

#include <vector>

namespace qttutorial::space {

class SatelliteTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column { NameColumn, PhaseColumn, BatteryColumn, EclipseColumn, HealthColumn, ColumnCount };

    explicit SatelliteTableModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setSatellites(std::vector<SatelliteState> satellites);
    [[nodiscard]] const SatelliteState* satelliteAt(int row) const;

private:
    std::vector<SatelliteState> m_satellites;
};

} // namespace qttutorial::space
