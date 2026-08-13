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

    void fieldsAreProgressPhaseShifted()
    {
        const FieldSample a = FieldOperationSimulator::sampleAt(0, 0.0);
        const FieldSample b = FieldOperationSimulator::sampleAt(1, 0.0);
        QVERIFY(a.coveragePercent != b.coveragePercent);
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
};

QTEST_MAIN(TestFieldOperationSimulator)
#include "test_field_operation_simulator.moc"
