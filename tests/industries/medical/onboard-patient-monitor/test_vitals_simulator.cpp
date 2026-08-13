// SPDX-License-Identifier: MIT
#include "VitalsSimulator.h"

#include <QTest>

using namespace qttutorial::medical;

class TestVitalsSimulator : public QObject {
    Q_OBJECT
private slots:
    void severityIsZeroOutsideEpisodeWindow()
    {
        QCOMPARE(VitalsSimulator::episodeSeverityAt(0.0), 0.0);
        QCOMPARE(VitalsSimulator::episodeSeverityAt(80.0), 0.0);
    }

    void severityRampsUpAndDownWithinEpisode()
    {
        QCOMPARE(VitalsSimulator::episodeSeverityAt(45.0), 0.5);
        QCOMPARE(VitalsSimulator::episodeSeverityAt(55.0), 1.0);
        QCOMPARE(VitalsSimulator::episodeSeverityAt(70.0), 0.5);
    }

    void heartRateRisesDuringEpisode()
    {
        const double normal = VitalsSimulator::heartRateAt(20.0);
        const double duringEpisode = VitalsSimulator::heartRateAt(55.0);
        QVERIFY(duringEpisode > normal + 20.0);
    }

    void spo2DropsDuringEpisode()
    {
        const double normal = VitalsSimulator::spo2At(20.0);
        const double duringEpisode = VitalsSimulator::spo2At(55.0);
        QVERIFY(duringEpisode < normal - 5.0);
    }

    void diastolicIsBelowSystolic()
    {
        for (double t = 0.0; t < 90.0; t += 5.0) {
            QVERIFY(VitalsSimulator::diastolicAt(t) < VitalsSimulator::systolicAt(t));
        }
    }

    void desaturationEventCorrelatesLowSpo2WithElevatedRespirationRate()
    {
        const double normalResp = VitalsSimulator::respirationRateAt(20.0);
        const double episodeResp = VitalsSimulator::respirationRateAt(55.0);
        QVERIFY(VitalsSimulator::spo2At(55.0) < VitalsSimulator::spo2At(20.0) - 5.0);
        QVERIFY(episodeResp > normalResp + 5.0);
    }

    void feverEpisodeIsZeroOutsideItsWindow()
    {
        QCOMPARE(VitalsSimulator::feverSeverityAt(20.0), 0.0);
        QCOMPARE(VitalsSimulator::feverSeverityAt(55.0), 0.0);
    }

    void feverEpisodeCorrelatesElevatedTemperatureWithElevatedHeartRateAndRespiration()
    {
        // t = 120 falls inside the fever window (100-140 within the 150s
        // fever cycle) at full severity, and also happens to land on an
        // integer multiple of the heart-rate and respiration wobble periods
        // (12s and 20s respectively), so the baseline contribution at this
        // exact instant is known: baseline heart rate is 72, baseline
        // respiration rate is 16 - any excess above that is the fever
        // contribution.
        QCOMPARE(VitalsSimulator::feverSeverityAt(120.0), 1.0);
        QVERIFY(VitalsSimulator::heartRateAt(120.0) > 80.0);
        QVERIFY(VitalsSimulator::temperatureAt(120.0) > 38.0);
        QVERIFY(VitalsSimulator::respirationRateAt(120.0) > 19.0);
    }
};

QTEST_MAIN(TestVitalsSimulator)
#include "test_vitals_simulator.moc"
