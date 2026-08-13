// SPDX-License-Identifier: GPL-3.0-or-later
#include "MachineListModel.h"
#include "MachineTelemetrySimulator.h"

#include <QSignalSpy>
#include <QTest>

using qttutorial::offboard_digital_twin::MachineListModel;
using qttutorial::offboard_digital_twin::MachineTelemetrySimulator;

class TestMachineListModel : public QObject {
    Q_OBJECT
private slots:
    void rowCountMatchesSimulator()
    {
        MachineTelemetrySimulator sim;
        MachineListModel model(&sim);
        QCOMPARE(model.rowCount(), sim.machineCount());
    }

    void dataReturnsExpectedRolesForFirstMachine()
    {
        MachineTelemetrySimulator sim;
        MachineListModel model(&sim);
        const QModelIndex idx = model.index(0, 0);

        QCOMPARE(model.data(idx, MachineListModel::IdRole).toInt(), sim.machineAt(0).id);
        QCOMPARE(model.data(idx, MachineListModel::NameRole).toString(), sim.machineAt(0).name);
        QCOMPARE(model.data(idx, MachineListModel::StateRole).toString(), QStringLiteral("normal"));
    }

    void indexOfMachineIdMatchesSimulator()
    {
        MachineTelemetrySimulator sim;
        MachineListModel model(&sim);
        QCOMPARE(model.indexOfMachineId(2), sim.indexOfId(2));
        QCOMPARE(model.indexOfMachineId(-1), -1);
    }

    void telemetryChangeEmitsDataChanged()
    {
        MachineTelemetrySimulator sim;
        MachineListModel model(&sim);
        QSignalSpy dataChangedSpy(&model, &MachineListModel::dataChanged);

        sim.tick(0.2);
        QVERIFY(dataChangedSpy.count() > 0);
    }
};

QTEST_MAIN(TestMachineListModel)
#include "test_machine_list_model.moc"
