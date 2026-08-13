// SPDX-License-Identifier: MIT
#include "FleetModel.h"

namespace qttutorial::automotive::fleet {

FleetModel::FleetModel(int vehicleCount, QObject* parent)
    : QAbstractTableModel(parent)
    , m_vehicleCount(vehicleCount)
{
    m_samples.reserve(static_cast<std::size_t>(vehicleCount));
    for (int i = 0; i < vehicleCount; ++i) {
        m_samples.push_back(FleetVehicleSimulator::sampleAt(i, 0.0));
    }

    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &FleetModel::tick);
    m_timer.start();
}

void FleetModel::tick()
{
    m_elapsedSeconds += 1.0;
    for (int i = 0; i < m_vehicleCount; ++i) {
        m_samples[static_cast<std::size_t>(i)] = FleetVehicleSimulator::sampleAt(i, m_elapsedSeconds);
    }
    emit dataChanged(index(0, 0), index(m_vehicleCount - 1, ColumnCount - 1));
    emit samplesUpdated();
}

int FleetModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_vehicleCount;
}

int FleetModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant FleetModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_vehicleCount) {
        return {};
    }
    const VehicleSample& sample = m_samples[static_cast<std::size_t>(index.row())];

    if (role == MaintenanceDueRole) {
        return sample.maintenanceDue;
    }
    if (role != Qt::DisplayRole) {
        return {};
    }
    switch (index.column()) {
    case IdColumn:
        return sample.id;
    case LocationColumn:
        return sample.location;
    case SpeedColumn:
        return QStringLiteral("%1 km/h").arg(sample.speedKph, 0, 'f', 1);
    case FuelColumn:
        return QStringLiteral("%1 %").arg(sample.fuelPercent, 0, 'f', 1);
    case EfficiencyColumn:
        return QStringLiteral("%1 %").arg(sample.efficiencyPercent, 0, 'f', 0);
    case MaintenanceColumn:
        return sample.maintenanceDue ? QStringLiteral("Due soon") : QStringLiteral("OK");
    case FaultCodesColumn:
        return sample.faultCodes.isEmpty() ? QStringLiteral("-") : sample.faultCodes.join(QStringLiteral(", "));
    default:
        return {};
    }
}

QVariant FleetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    switch (section) {
    case IdColumn:
        return QStringLiteral("Vehicle");
    case LocationColumn:
        return QStringLiteral("Location");
    case SpeedColumn:
        return QStringLiteral("Speed");
    case FuelColumn:
        return QStringLiteral("Fuel");
    case EfficiencyColumn:
        return QStringLiteral("Efficiency");
    case MaintenanceColumn:
        return QStringLiteral("Maintenance");
    case FaultCodesColumn:
        return QStringLiteral("Fault codes");
    default:
        return {};
    }
}

VehicleSample FleetModel::sampleAt(int row) const
{
    if (row < 0 || row >= m_vehicleCount) {
        return {};
    }
    return m_samples[static_cast<std::size_t>(row)];
}

} // namespace qttutorial::automotive::fleet
