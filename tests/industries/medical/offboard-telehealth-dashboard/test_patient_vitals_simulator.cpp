// SPDX-License-Identifier: MIT
#include "PatientVitalsSimulator.h"

#include <QTest>

using namespace qttutorial::medical::telehealth;

class TestPatientVitalsSimulator : public QObject {
    Q_OBJECT
private slots:
    void producesDistinctIdsPerPatient()
    {
        const PatientVitals a = PatientVitalsSimulator::sampleAt(0, 0.0);
        const PatientVitals b = PatientVitalsSimulator::sampleAt(1, 0.0);
        QVERIFY(a.id != b.id);
        QVERIFY(a.name != b.name);
    }

    void firstPatientDeterioratesPeriodically()
    {
        const PatientVitals normal = PatientVitalsSimulator::sampleAt(0, 20.0);
        const PatientVitals episode = PatientVitalsSimulator::sampleAt(0, 55.0);
        QVERIFY(episode.heartRate > normal.heartRate + 20.0);
        QVERIFY(episode.spo2 < normal.spo2 - 5.0);
    }

    void otherPatientsStayStable()
    {
        const PatientVitals sample = PatientVitalsSimulator::sampleAt(2, 55.0);
        QVERIFY(sample.heartRate < 100.0);
        QVERIFY(sample.spo2 > 90.0);
    }

    void desaturationEventCorrelatesLowSpo2WithElevatedRespirationRate()
    {
        const PatientVitals normal = PatientVitalsSimulator::sampleAt(0, 20.0);
        const PatientVitals episode = PatientVitalsSimulator::sampleAt(0, 55.0);
        QVERIFY(episode.spo2 < normal.spo2 - 5.0);
        QVERIFY(episode.respirationRate > normal.respirationRate + 2.0);
    }

    void secondPatientHasFeverEpisodeCorrelatingTemperatureWithHeartRateAndRespiration()
    {
        // Patient index 1 runs a fever episode on its own, longer, offset
        // cycle - distinct from patient 0's desaturation event.
        const PatientVitals normal = PatientVitalsSimulator::sampleAt(1, 20.0);
        const PatientVitals fever = PatientVitalsSimulator::sampleAt(1, 120.0);
        QVERIFY(fever.temperature > normal.temperature + 1.0);
        QVERIFY(fever.heartRate > normal.heartRate + 8.0);
        QVERIFY(fever.respirationRate > normal.respirationRate + 2.0);
        // Fever does not desaturate the patient the way the index-0 episode
        // does.
        QVERIFY(fever.spo2 > 90.0);
    }

    void classifyEscalatesWithVitals()
    {
        PatientVitals healthy;
        healthy.heartRate = 72.0;
        healthy.spo2 = 97.0;
        QCOMPARE(PatientVitalsSimulator::classify(healthy), AlarmSeverity::Normal);

        PatientVitals warning;
        warning.heartRate = 115.0;
        warning.spo2 = 97.0;
        QCOMPARE(PatientVitalsSimulator::classify(warning), AlarmSeverity::Warning);

        PatientVitals critical;
        critical.heartRate = 130.0;
        critical.spo2 = 85.0;
        QCOMPARE(PatientVitalsSimulator::classify(critical), AlarmSeverity::Critical);
    }
};

QTEST_MAIN(TestPatientVitalsSimulator)
#include "test_patient_vitals_simulator.moc"
