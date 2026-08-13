// SPDX-License-Identifier: MIT
#include "FleetTableModel.h"

#include <QColor>

namespace qttutorial::avionics {

FleetTableModel::FleetTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int FleetTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_aircraft.size());
}

int FleetTableModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant FleetTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_aircraft.size())) {
        return {};
    }
    const Aircraft& aircraft = m_aircraft[static_cast<std::size_t>(index.row())];

    if (role == Qt::BackgroundRole && index.column() == StatusColumn && aircraft.maintenanceFlag) {
        return QColor(0xff, 0xd0, 0x80);
    }

    if (role != Qt::DisplayRole) {
        return {};
    }
    switch (index.column()) {
    case TailColumn:
        return aircraft.tailNumber;
    case TypeColumn:
        return aircraft.type;
    case VibrationColumn:
        return QString::number(aircraft.engine1.vibrationIps, 'f', 3);
    case EgtMarginColumn:
        return QString::number(aircraft.engine1.egtMarginC, 'f', 1);
    case OilPressureColumn:
        return QString::number(aircraft.engine1.oilPressurePsi, 'f', 1);
    case StatusColumn:
        return aircraft.maintenanceFlag ? QStringLiteral("Inspect") : QStringLiteral("Nominal");
    default:
        return {};
    }
}

QVariant FleetTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    switch (section) {
    case TailColumn:
        return QStringLiteral("Tail");
    case TypeColumn:
        return QStringLiteral("Type");
    case VibrationColumn:
        return QStringLiteral("Eng1 Vib (ips)");
    case EgtMarginColumn:
        return QStringLiteral("Eng1 EGT Margin (C)");
    case OilPressureColumn:
        return QStringLiteral("Eng1 Oil (psi)");
    case StatusColumn:
        return QStringLiteral("Status");
    default:
        return {};
    }
}

void FleetTableModel::setAircraft(std::vector<Aircraft> aircraft)
{
    beginResetModel();
    m_aircraft = std::move(aircraft);
    endResetModel();
}

const Aircraft* FleetTableModel::aircraftAt(int row) const
{
    if (row < 0 || row >= static_cast<int>(m_aircraft.size())) {
        return nullptr;
    }
    return &m_aircraft[static_cast<std::size_t>(row)];
}

} // namespace qttutorial::avionics
