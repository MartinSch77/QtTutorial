// SPDX-License-Identifier: MIT
#include "InspectionScheduler.h"

#include <QTest>

using namespace qttutorial::avionics;

class TestInspectionScheduler : public QObject {
    Q_OBJECT
private slots:
    void freshAircraftIsNominal()
    {
        InspectionScheduler scheduler;
        const auto status = scheduler.evaluate(0.0, 0, false);
        QCOMPARE(status.urgency, InspectionUrgency::Nominal);
        QCOMPARE(status.hoursRemaining, InspectionScheduler::kIntervalHours);
        QCOMPARE(status.cyclesRemaining, InspectionScheduler::kIntervalCycles);
    }

    void closeToIntervalWithoutAnomalyIsDueSoonNotUrgent()
    {
        InspectionScheduler scheduler;
        const auto status =
            scheduler.evaluate(InspectionScheduler::kIntervalHours - 10.0, 0, false);
        QCOMPARE(status.urgency, InspectionUrgency::DueSoon);
    }

    void closeToIntervalWithAnomalyEscalatesToUrgent()
    {
        // This is the correlation the class exists to express: the same
        // "due soon" utilization becomes "urgent" once there is also an
        // active engine-parameter anomaly on that aircraft.
        InspectionScheduler scheduler;
        const auto withoutAnomaly =
            scheduler.evaluate(InspectionScheduler::kIntervalHours - 10.0, 0, false);
        const auto withAnomaly =
            scheduler.evaluate(InspectionScheduler::kIntervalHours - 10.0, 0, true);
        QCOMPARE(withoutAnomaly.urgency, InspectionUrgency::DueSoon);
        QCOMPARE(withAnomaly.urgency, InspectionUrgency::Urgent);
    }

    void pastIntervalIsAlwaysUrgent()
    {
        InspectionScheduler scheduler;
        const auto status = scheduler.evaluate(InspectionScheduler::kIntervalHours + 5.0, 0, false);
        QCOMPARE(status.urgency, InspectionUrgency::Urgent);
        QCOMPARE(status.hoursRemaining, 0.0);
    }

    void cyclesDriveUrgencyIndependentlyOfHours()
    {
        InspectionScheduler scheduler;
        const auto status =
            scheduler.evaluate(0.0, InspectionScheduler::kIntervalCycles - 5, false);
        QCOMPARE(status.urgency, InspectionUrgency::DueSoon);
    }
};

QTEST_MAIN(TestInspectionScheduler)
#include "test_inspection_scheduler.moc"
