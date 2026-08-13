// SPDX-License-Identifier: MIT
#include "FleetSimulator.h"

#include <QTest>

#include <algorithm>
#include <vector>

using namespace qttutorial::avionics;

class TestFleetInspectionCorrelation : public QObject {
    Q_OBJECT
private slots:
    void aircraftFarFromInspectionWithNoAnomalyStaysNominal()
    {
        FleetSimulator simulator;
        // N101QT (index 0) is seeded far from its inspection interval and has
        // no seeded drift fault.
        for (int i = 0; i < 10; ++i) {
            simulator.advance(0.5);
        }
        const auto status = simulator.inspectionStatus(0);
        QCOMPARE(status.urgency, InspectionUrgency::Nominal);
    }

    void aircraftDueSoonWithEngineAnomalyEscalatesToUrgent()
    {
        // N102QT (index 1) is seeded already "due soon" and carries the
        // seeded vibration-drift fault, so once the drift trend flags it,
        // InspectionScheduler should escalate it to Urgent - the concrete
        // correlation between an engine-parameter anomaly and a
        // maintenance-due condition that this simulator is meant to
        // demonstrate.
        FleetSimulator simulator;
        std::vector<QString> priorities;
        QObject::connect(&simulator, &FleetSimulator::maintenanceTaskCreated,
                          [&priorities](const MaintenanceTask& task) { priorities.push_back(task.priority); });

        bool becameUrgentWhileNotYetOverdue = false;
        for (int i = 0; i < 2000; ++i) {
            simulator.advance(0.5);
            const auto status = simulator.inspectionStatus(1);
            if (status.urgency == InspectionUrgency::Urgent) {
                // Confirm this is the anomaly-driven escalation, not the
                // separate "ran past the interval outright" path: at this
                // point the aircraft must still have time/cycles left, and
                // the only reason it is Urgent rather than just DueSoon is
                // the active engine anomaly.
                becameUrgentWhileNotYetOverdue = status.hoursRemaining > 0.0 && status.cyclesRemaining > 0;
                break;
            }
        }
        QVERIFY(becameUrgentWhileNotYetOverdue);
        QVERIFY(simulator.aircraft().at(1).maintenanceFlag);

        const bool sawUrgentTask =
            std::find(priorities.begin(), priorities.end(), QStringLiteral("Urgent")) != priorities.end();
        QVERIFY(sawUrgentTask);
    }
};

QTEST_MAIN(TestFleetInspectionCorrelation)
#include "test_fleet_inspection_correlation.moc"
