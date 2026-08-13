// SPDX-License-Identifier: MIT
#include "FieldOperationSimulator.h"

#include <QTest>

using namespace qttutorial::agriculture::ops;

class TestFieldOperationSimulator : public QObject {
    Q_OBJECT
private slots:
    void producesDistinctIdsPerField()
    {
        const FieldSample a = FieldOperationSimulator::sampleAt(0, 0.0);
        const FieldSample b = FieldOperationSimulator::sampleAt(1, 0.0);
        QVERIFY(a.fieldId != b.fieldId);
        QVERIFY(a.vehicleId != b.vehicleId);
        QCOMPARE(a.fieldId, QStringLiteral("FLD-001"));
    }

    void fieldsAreDispatchedInStaggeredSequenceNotInLockstep()
    {
        // Field 1 is dispatched kDispatchDelaySecondsPerField seconds after
        // field 0, so partway through that delay window field 0 has already
        // started working while field 1 has not been dispatched yet.
        const double midDelay = FieldOperationSimulator::kDispatchDelaySecondsPerField / 2.0;
        const FieldSample a = FieldOperationSimulator::sampleAt(0, midDelay);
        const FieldSample b = FieldOperationSimulator::sampleAt(1, midDelay);
        QVERIFY(a.overallStatus != QStringLiteral("not_started"));
        QCOMPARE(b.overallStatus, QStringLiteral("not_started"));
        QCOMPARE(b.status, QStringLiteral("scheduled"));
        QCOMPARE(b.coveragePercent, 0.0);
    }

    void coverageStaysWithinValidRange()
    {
        for (int field = 0; field < 6; ++field) {
            for (double t = 0.0; t < 1000.0; t += 5.0) {
                const FieldSample sample = FieldOperationSimulator::sampleAt(field, t);
                QVERIFY(sample.coveragePercent >= 0.0);
                QVERIFY(sample.coveragePercent <= 100.0);
            }
        }
    }

    void isIdleAtStartOfPass()
    {
        const FieldSample sample = FieldOperationSimulator::sampleAt(0, 0.0);
        QCOMPARE(sample.status, QStringLiteral("idle"));
        QCOMPARE(sample.coveragePercent, 0.0);
    }

    void isTurningNearRowEnds()
    {
        const FieldSample nearStart = FieldOperationSimulator::sampleAt(0, 20.0);
        QCOMPARE(nearStart.status, QStringLiteral("turning"));

        const FieldSample nearEnd = FieldOperationSimulator::sampleAt(0, 235.0);
        QCOMPARE(nearEnd.status, QStringLiteral("turning"));
    }

    void isWorkingInTheMiddleOfThePass()
    {
        const FieldSample sample = FieldOperationSimulator::sampleAt(0, 120.0);
        QCOMPARE(sample.status, QStringLiteral("working"));
    }

    void passNumberIncrementsAcrossPeriods()
    {
        const FieldSample firstPass = FieldOperationSimulator::sampleAt(0, 10.0);
        const FieldSample secondPass = FieldOperationSimulator::sampleAt(0, 250.0);
        QCOMPARE(firstPass.passNumber, 1);
        QCOMPARE(secondPass.passNumber, 2);
    }

    void plannedPassesVaryByField()
    {
        const int passesA = FieldOperationSimulator::plannedPassesFor(0);
        const int passesB = FieldOperationSimulator::plannedPassesFor(1);
        QVERIFY(passesA >= 2);
        QVERIFY(passesB >= 2);
        QVERIFY(passesA != passesB);
    }

    void fieldBecomesCompleteOnceAllPlannedPassesAreDone()
    {
        const int totalPasses = FieldOperationSimulator::plannedPassesFor(0);
        const double totalWorkSeconds = totalPasses * FieldOperationSimulator::kPassPeriodSeconds;

        const FieldSample midway = FieldOperationSimulator::sampleAt(0, totalWorkSeconds / 2.0);
        QCOMPARE(midway.overallStatus, QStringLiteral("in_progress"));
        QVERIFY(midway.overallFieldProgressPercent > 0.0);
        QVERIFY(midway.overallFieldProgressPercent < 100.0);

        const FieldSample done = FieldOperationSimulator::sampleAt(0, totalWorkSeconds + 1.0);
        QCOMPARE(done.overallStatus, QStringLiteral("complete"));
        QCOMPARE(done.status, QStringLiteral("complete"));
        QCOMPARE(done.coveragePercent, 100.0);
        QCOMPARE(done.overallFieldProgressPercent, 100.0);
    }

    void engineLoadIsHigherWhenWorkingThanIdleOrTurning()
    {
        const double idleLoad = FieldOperationSimulator::engineLoadForStatus(QStringLiteral("idle"));
        const double turningLoad = FieldOperationSimulator::engineLoadForStatus(QStringLiteral("turning"));
        const double workingLoad = FieldOperationSimulator::engineLoadForStatus(QStringLiteral("working"));
        QVERIFY(workingLoad > turningLoad);
        QVERIFY(turningLoad > idleLoad);
    }

    void fuelBurnRateIncreasesMonotonicallyWithEngineLoad()
    {
        double lastRate = -1.0;
        for (double load = 0.0; load <= 100.0; load += 5.0) {
            const double rate = FieldOperationSimulator::fuelBurnPercentPerSecondAt(load);
            QVERIFY(rate > lastRate);
            lastRate = rate;
        }
    }

    void fuelLevelDecreasesMonotonicallyAsFieldWorkProgresses()
    {
        double lastFuel = 101.0;
        for (double t = 0.0; t < 900.0; t += 30.0) {
            const FieldSample sample = FieldOperationSimulator::sampleAt(0, t);
            QVERIFY(sample.fuelLevelPercent <= lastFuel + 1e-9);
            QVERIFY(sample.fuelLevelPercent >= FieldOperationSimulator::kFuelFloorPercent - 1e-9);
            lastFuel = sample.fuelLevelPercent;
        }
    }
};

QTEST_MAIN(TestFieldOperationSimulator)
#include "test_field_operation_simulator.moc"
