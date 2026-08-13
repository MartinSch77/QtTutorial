// SPDX-License-Identifier: MIT
#pragma once

#include "FleetVehicleSimulator.h"

#include <QAbstractTableModel>
#include <QTimer>

#include <vector>

namespace qttutorial::automotive::fleet {

// A QAbstractTableModel fed by FleetVehicleSimulator on a QTimer, standing in
// for a telemetry-ingestion service pushing updates into a fleet back office.
class FleetModel : public QAbstractTableModel {
    Q_OBJECT
public:
    enum Column {
        IdColumn,
        LocationColumn,
        SpeedColumn,
        FuelColumn,
        EfficiencyColumn,
        MaintenanceColumn,
        FaultCodesColumn,
        ColumnCount
    };

    // Role used by FleetMaintenanceIconDelegate to fetch the raw maintenance-due
    // flag for the hand-painted wrench icon, independent of the display text.
    static constexpr int MaintenanceDueRole = Qt::UserRole + 1;

    explicit FleetModel(int vehicleCount = 8, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    [[nodiscard]] VehicleSample sampleAt(int row) const;

signals:
    void samplesUpdated();

private:
    void tick();

    int m_vehicleCount;
    double m_elapsedSeconds = 0.0;
    std::vector<VehicleSample> m_samples;
    QTimer m_timer;
};

} // namespace qttutorial::automotive::fleet
