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
};

QTEST_MAIN(TestVitalsSimulator)
#include "test_vitals_simulator.moc"
