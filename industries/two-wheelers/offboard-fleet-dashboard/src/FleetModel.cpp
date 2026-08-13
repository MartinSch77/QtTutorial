// SPDX-License-Identifier: MIT
#include "FleetModel.h"

namespace qttutorial::two_wheelers::fleet {

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

    if (role == Qt::UserRole) {
        // Raw values for FleetIconDelegate, which needs the numeric/boolean
        // value rather than the formatted display text.
        switch (index.column()) {
        case BatteryColumn:
            return sample.batteryPercent;
        case MaintenanceColumn:
            return sample.maintenanceDue;
        default:
            return {};
        }
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
    case BatteryColumn:
        return QStringLiteral("%1 %").arg(sample.batteryPercent, 0, 'f', 1);
    case StatusColumn:
        return riderStatusLabel(sample.status);
    case OdometerColumn:
        return QStringLiteral("%1 km").arg(sample.odometerKm, 0, 'f', 0);
    case MaintenanceColumn:
        return sample.maintenanceDue ? QStringLiteral("Due") : QStringLiteral("OK");
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
    case BatteryColumn:
        return QStringLiteral("Battery");
    case StatusColumn:
        return QStringLiteral("Status");
    case OdometerColumn:
        return QStringLiteral("Odometer");
    case MaintenanceColumn:
        return QStringLiteral("Service");
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

} // namespace qttutorial::two_wheelers::fleet
