// SPDX-License-Identifier: MIT
#include "RespirationWaveformGenerator.h"

#include <QTest>

using namespace qttutorial::medical;

class TestRespirationWaveformGenerator : public QObject {
    Q_OBJECT
private slots:
    void peaksAtEndOfInspiration()
    {
        // Inspiration is the first 40% of the cycle; the sample should be
        // near its maximum there and much lower at the very start/end of the
        // cycle (full exhalation).
        const double atInspirationEnd = RespirationWaveformGenerator::sample(0.4 * 1.0, 60.0);
        const double atCycleStart = RespirationWaveformGenerator::sample(0.0, 60.0);
        QVERIFY(atInspirationEnd > atCycleStart);
        QVERIFY(atInspirationEnd > 0.9);
    }

    void waveformRepeatsEachBreathCycle()
    {
        const double cycleLength = 60.0 / 12.0; // 12 breaths/min
        const double first = RespirationWaveformGenerator::sample(1.0, 12.0);
        const double second = RespirationWaveformGenerator::sample(1.0 + cycleLength, 12.0);
        QVERIFY(qFuzzyCompare(first + 1.0, second + 1.0));
    }

    void higherRespirationRateShortensCycle()
    {
        // At t = 0.4s, a slow (12 bpm, 5s cycle) breath is still early in
        // inspiration, while a fast (60 bpm, 1s cycle) breath has already
        // completed several full cycles and is at the very start again.
        const double slow = RespirationWaveformGenerator::sample(0.4, 12.0);
        const double fastSameTime = RespirationWaveformGenerator::sample(0.4, 60.0);
        QVERIFY(slow < fastSameTime);
    }

    void staysWithinExpectedRange()
    {
        for (double t = 0.0; t < 5.0; t += 0.05) {
            const double value = RespirationWaveformGenerator::sample(t, 16.0);
            QVERIFY(value >= -1.01 && value <= 1.01);
        }
    }
};

QTEST_MAIN(TestRespirationWaveformGenerator)
#include "test_respiration_waveform_generator.moc"
