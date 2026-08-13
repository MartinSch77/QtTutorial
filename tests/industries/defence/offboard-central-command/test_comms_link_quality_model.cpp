// SPDX-License-Identifier: MIT
#include "CommsLinkQualityModel.h"

#include <QTest>

using namespace qttutorial::defence;

class TestCommsLinkQualityModel : public QObject {
    Q_OBJECT
private slots:
    void fullQualityUpdatesEveryTick() { QCOMPARE(commsLinkUpdateIntervalTicks(100.0), 1); }

    void zeroQualityIsMoreIntermittentThanFullQuality()
    {
        QVERIFY(commsLinkUpdateIntervalTicks(0.0) > commsLinkUpdateIntervalTicks(100.0));
    }

    void isMonotonicNonIncreasingAsQualityImproves()
    {
        int previousInterval = commsLinkUpdateIntervalTicks(0.0);
        for (double quality : {10.0, 30.0, 50.0, 70.0, 90.0, 100.0}) {
            const int interval = commsLinkUpdateIntervalTicks(quality);
            QVERIFY(interval <= previousInterval);
            previousInterval = interval;
        }
    }

    void clampsOutOfRangeInputs()
    {
        QCOMPARE(commsLinkUpdateIntervalTicks(150.0), commsLinkUpdateIntervalTicks(100.0));
        QCOMPARE(commsLinkUpdateIntervalTicks(-50.0), commsLinkUpdateIntervalTicks(0.0));
    }
};

QTEST_MAIN(TestCommsLinkQualityModel)
#include "test_comms_link_quality_model.moc"
