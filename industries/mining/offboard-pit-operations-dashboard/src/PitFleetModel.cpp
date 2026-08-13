// SPDX-License-Identifier: MIT
#include "PitFleetModel.h"

#include "PitIcons.h"

namespace qttutorial::mining::pit {

PitFleetModel::PitFleetModel(int truckCount, QObject* parent)
    : QAbstractTableModel(parent)
    , m_truckCount(truckCount)
{
    m_samples.reserve(static_cast<std::size_t>(truckCount));
    for (int i = 0; i < truckCount; ++i) {
        m_samples.push_back(HaulFleetSimulator::sampleAt(i, 0.0));
    }

    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &PitFleetModel::tick);
    m_timer.start();
}

void PitFleetModel::tick()
{
    m_elapsedSeconds += 1.0;
    for (int i = 0; i < m_truckCount; ++i) {
        m_samples[static_cast<std::size_t>(i)] = HaulFleetSimulator::sampleAt(i, m_elapsedSeconds);
    }
    emit dataChanged(index(0, 0), index(m_truckCount - 1, ColumnCount - 1));
    emit samplesUpdated();
}

int PitFleetModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_truckCount;
}

int PitFleetModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant PitFleetModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_truckCount) {
        return {};
    }
    const TruckSample& sample = m_samples[static_cast<std::size_t>(index.row())];

    if (role == Qt::ToolTipRole && sample.overloaded) {
        return QStringLiteral("Payload exceeds rated capacity of %1 t")
            .arg(HaulFleetSimulator::kRatedCapacityTonnes);
    }
    if (role == Qt::DecorationRole && index.column() == IdColumn && sample.overloaded) {
        return icons::warningTriangleIcon(QColor("#e5484d"), 16);
    }

    if (role != Qt::DisplayRole) {
        return {};
    }
    switch (index.column()) {
    case IdColumn:
        return sample.id;
    case StateColumn:
        return sample.stateLabel;
    case PayloadColumn:
        return QStringLiteral("%1 t").arg(sample.payloadTonnes, 0, 'f', 1);
    case SpeedColumn:
        return QStringLiteral("%1 km/h").arg(sample.speedKph, 0, 'f', 0);
    case FuelColumn:
        return QStringLiteral("%1 L/h").arg(sample.fuelLtrPerHour, 0, 'f', 0);
    case LocationColumn:
        return sample.location;
    default:
        return {};
    }
}

QVariant PitFleetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    switch (section) {
    case IdColumn:
        return QStringLiteral("Truck");
    case StateColumn:
        return QStringLiteral("Haul state");
    case PayloadColumn:
        return QStringLiteral("Payload");
    case SpeedColumn:
        return QStringLiteral("Speed");
    case FuelColumn:
        return QStringLiteral("Fuel");
    case LocationColumn:
        return QStringLiteral("Location");
    default:
        return {};
    }
}

TruckSample PitFleetModel::sampleAt(int row) const
{
    if (row < 0 || row >= m_truckCount) {
        return {};
    }
    return m_samples[static_cast<std::size_t>(row)];
}

} // namespace qttutorial::mining::pit
