// SPDX-License-Identifier: MIT
#include "EngineTrend.h"

#include <QTest>

using namespace qttutorial::avionics;

class TestEngineTrend : public QObject {
    Q_OBJECT
private slots:
    void staysNominalWithoutSamples()
    {
        EngineTrend trend(0.0, 1.0, 5);
        QVERIFY(!trend.isOutOfBand());
        QVERIFY(!trend.isDrifting());
    }

    void flagsOutOfBandImmediately()
    {
        EngineTrend trend(0.0, 1.0, 5);
        trend.addSample(0.5);
        trend.addSample(1.5);
        QVERIFY(trend.isOutOfBand());
    }

    void doesNotFlagStableInBandSeries()
    {
        EngineTrend trend(0.0, 1.0, 10);
        for (int i = 0; i < 10; ++i) {
            trend.addSample(0.5);
        }
        QVERIFY(!trend.isOutOfBand());
        QVERIFY(!trend.isDrifting());
    }

    void flagsProjectedDriftBeforeCrossingBand()
    {
        EngineTrend trend(0.0, 1.0, 10);
        for (int i = 0; i < 10; ++i) {
            trend.addSample(0.5 + 0.04 * i);
        }
        QVERIFY(!trend.isOutOfBand());
        QVERIFY(trend.isDrifting());
        QVERIFY(trend.slopePerSample() > 0.0);
    }

    void windowIsBounded()
    {
        EngineTrend trend(0.0, 1.0, 3);
        for (int i = 0; i < 10; ++i) {
            trend.addSample(static_cast<double>(i));
        }
        QCOMPARE(trend.sampleCount(), std::size_t(3));
    }
};

QTEST_MAIN(TestEngineTrend)
#include "test_engine_trend.moc"
