// SPDX-License-Identifier: MIT
#include "EcgWaveformGenerator.h"

#include <QTest>

using namespace qttutorial::medical;

class TestEcgWaveformGenerator : public QObject {
    Q_OBJECT
private slots:
    void rSpikeIsTheDominantFeature()
    {
        const double atRPeak = EcgWaveformGenerator::sample(0.30, 60.0);
        const double atBaseline = EcgWaveformGenerator::sample(0.0, 60.0);
        const double atTWave = EcgWaveformGenerator::sample(0.55, 60.0);
        QVERIFY(atRPeak > atBaseline);
        QVERIFY(atRPeak > atTWave);
        QVERIFY(atRPeak > 0.5);
    }

    void waveformRepeatsEachCardiacCycle()
    {
        const double first = EcgWaveformGenerator::sample(0.1, 60.0);
        const double second = EcgWaveformGenerator::sample(1.1, 60.0);
        QVERIFY(qFuzzyCompare(first + 1.0, second + 1.0));
    }

    void higherHeartRateShortensCycle()
    {
        const double slowPeak = EcgWaveformGenerator::sample(0.30, 60.0);
        const double fastPeakSameTime = EcgWaveformGenerator::sample(0.30, 120.0);
        QVERIFY(slowPeak > fastPeakSameTime);
    }
};

QTEST_MAIN(TestEcgWaveformGenerator)
#include "test_ecg_waveform_generator.moc"
