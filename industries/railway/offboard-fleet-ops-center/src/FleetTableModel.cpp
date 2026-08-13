// SPDX-License-Identifier: MIT
#include "FleetTableModel.h"
#include "DelayCalculator.h"

namespace qttutorial::fleet_ops {

FleetTableModel::FleetTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

int FleetTableModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_states.size());
}

int FleetTableModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant FleetTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_states.size()) || role != Qt::DisplayRole) {
        return {};
    }
    const TrainState& state = m_states[static_cast<std::size_t>(index.row())];
    switch (index.column()) {
    case TrainIdColumn:
        return state.trainId;
    case PositionColumn:
        return QString::number(state.positionKm, 'f', 1) + QStringLiteral(" km");
    case SpeedColumn:
        return QString::number(state.speedKmh, 'f', 0) + QStringLiteral(" km/h");
    case DelayColumn:
        return QString::number(state.delayMinutes, 'f', 1) + QStringLiteral(" min");
    case NextStopColumn:
        return QStringLiteral("Stop %1 (%2 km)")
            .arg(state.nextStopIndex)
            .arg(QString::number(state.distanceToNextStopKm, 'f', 1));
    case StatusColumn:
        return QString::fromLatin1(punctualityLabel(classifyDelay(state.delayMinutes)));
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
    case TrainIdColumn:
        return QStringLiteral("Train");
    case PositionColumn:
        return QStringLiteral("Position");
    case SpeedColumn:
        return QStringLiteral("Speed");
    case DelayColumn:
        return QStringLiteral("Delay");
    case NextStopColumn:
        return QStringLiteral("Next stop");
    case StatusColumn:
        return QStringLiteral("Status");
    default:
        return {};
    }
}

void FleetTableModel::setStates(std::vector<TrainState> states)
{
    beginResetModel();
    m_states = std::move(states);
    endResetModel();
}

} // namespace qttutorial::fleet_ops
