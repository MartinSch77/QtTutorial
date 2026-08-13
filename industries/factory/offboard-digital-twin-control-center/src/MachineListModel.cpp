// SPDX-License-Identifier: GPL-3.0-or-later
#include "MachineListModel.h"

namespace qttutorial::offboard_digital_twin {

namespace {
QString stateToString(MachineState state)
{
    switch (state) {
    case MachineState::Normal: return QStringLiteral("normal");
    case MachineState::Warning: return QStringLiteral("warning");
    case MachineState::Alarm: return QStringLiteral("alarm");
    case MachineState::Stopped: return QStringLiteral("stopped");
    case MachineState::Maintenance: return QStringLiteral("maintenance");
    }
    return QStringLiteral("normal");
}
}

MachineListModel::MachineListModel(MachineTelemetrySimulator *simulator, QObject *parent)
    : QAbstractListModel(parent)
    , m_simulator(simulator)
{
    connect(m_simulator, &MachineTelemetrySimulator::machineTelemetryChanged, this,
            [this](int machineId) {
                const int row = m_simulator->indexOfId(machineId);
                if (row < 0)
                    return;
                const QModelIndex idx = index(row, 0);
                emit dataChanged(idx, idx);
            });
}

int MachineListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_simulator->machineCount();
}

QVariant MachineListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_simulator->machineCount())
        return {};

    const MachineTelemetry &m = m_simulator->machineAt(index.row());
    switch (role) {
    case IdRole: return m.id;
    case NameRole: return m.name;
    case KindRole: return m.kind;
    case PositionRole: return QVariant::fromValue(m.position);
    case TemperatureRole: return m.temperatureC;
    case VibrationRole: return m.vibrationMm;
    case SpeedRole: return m.speedRpm;
    case StateRole: return stateToString(m.state);
    case DefectivePartRole: return m.defectivePart;
    default: return {};
    }
}

QHash<int, QByteArray> MachineListModel::roleNames() const
{
    return {
        {IdRole, "machineId"},
        {NameRole, "name"},
        {KindRole, "kind"},
        {PositionRole, "position"},
        {TemperatureRole, "temperature"},
        {VibrationRole, "vibration"},
        {SpeedRole, "speed"},
        {StateRole, "state"},
        {DefectivePartRole, "defectivePart"},
    };
}

int MachineListModel::indexOfMachineId(int machineId) const
{
    return m_simulator->indexOfId(machineId);
}

} // namespace qttutorial::offboard_digital_twin
