// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "MachineTelemetrySimulator.h"

#include <QAbstractListModel>

namespace qttutorial::offboard_digital_twin {

// Thin QAbstractListModel adapter over MachineTelemetrySimulator, so both the
// left-hand equipment tree (ListView) and the 3D scene's Repeater can bind to
// the same live data. Kept separate from the simulator itself so the
// simulator stays a plain, QAbstractListModel-free QObject that is trivial to
// unit test without a QML engine.
class MachineListModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        IdRole = Qt::UserRole + 1,
        NameRole,
        KindRole,
        PositionRole,
        TemperatureRole,
        VibrationRole,
        SpeedRole,
        StateRole,
        DefectivePartRole,
    };

    explicit MachineListModel(MachineTelemetrySimulator *simulator, QObject *parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE int indexOfMachineId(int machineId) const;

private:
    MachineTelemetrySimulator *m_simulator;
};

} // namespace qttutorial::offboard_digital_twin
