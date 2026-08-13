// SPDX-License-Identifier: MIT
#include "SatelliteTableModel.h"

#include <QColor>

namespace qttutorial::space {

SatelliteTableModel::SatelliteTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int SatelliteTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_satellites.size());
}

int SatelliteTableModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant SatelliteTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_satellites.size())) {
        return {};
    }
    const SatelliteState& satellite = m_satellites[static_cast<std::size_t>(index.row())];

    if (role == Qt::BackgroundRole && index.column() == HealthColumn) {
        switch (satellite.health) {
        case HealthSummary::Nominal:
            return QColor(0x2a, 0x8a, 0x2a);
        case HealthSummary::Caution:
            return QColor(0xb8, 0x86, 0x0b);
        case HealthSummary::Critical:
            return QColor(0xa0, 0x20, 0x20);
        }
    }

    if (role != Qt::DisplayRole) {
        return {};
    }
    switch (index.column()) {
    case NameColumn:
        return satellite.name;
    case PhaseColumn:
        return QString::number(satellite.phaseDeg, 'f', 1);
    case BatteryColumn:
        return QString::number(satellite.batteryPercent, 'f', 1);
    case EclipseColumn:
        return satellite.inEclipse ? QStringLiteral("Eclipse") : QStringLiteral("Sunlit");
    case HealthColumn:
        return toString(satellite.health);
    default:
        return {};
    }
}

QVariant SatelliteTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    switch (section) {
    case NameColumn:
        return QStringLiteral("Satellite");
    case PhaseColumn:
        return QStringLiteral("Orbit Phase (deg)");
    case BatteryColumn:
        return QStringLiteral("Battery (%)");
    case EclipseColumn:
        return QStringLiteral("Sun State");
    case HealthColumn:
        return QStringLiteral("Health");
    default:
        return {};
    }
}

void SatelliteTableModel::setSatellites(std::vector<SatelliteState> satellites)
{
    beginResetModel();
    m_satellites = std::move(satellites);
    endResetModel();
}

const SatelliteState* SatelliteTableModel::satelliteAt(int row) const
{
    if (row < 0 || row >= static_cast<int>(m_satellites.size())) {
        return nullptr;
    }
    return &m_satellites[static_cast<std::size_t>(row)];
}

} // namespace qttutorial::space
