// SPDX-License-Identifier: MIT
#include "PriceSimulator.h"

#include <QTest>

using qttutorial::stock_tracker::PriceSimulator;

class TestPriceSimulator : public QObject {
    Q_OBJECT
private slots:
    void staysWithinBoundsOverManySteps()
    {
        PriceSimulator sim(QStringLiteral("QTX"), 100.0, 80.0, 120.0, 42);
        for (int i = 0; i < 5000; ++i) {
            const PriceSimulator::Sample sample = sim.next();
            QVERIFY(sample.price >= 80.0);
            QVERIFY(sample.price <= 120.0);
        }
    }

    void sequenceIncrementsByOneEachStep()
    {
        PriceSimulator sim(QStringLiteral("QTX"), 100.0, 80.0, 120.0, 42);
        qint64 previous = 0;
        for (int i = 0; i < 20; ++i) {
            const PriceSimulator::Sample sample = sim.next();
            QCOMPARE(sample.sequence, previous + 1);
            previous = sample.sequence;
        }
    }

    void sameSeedProducesIdenticalSequence()
    {
        PriceSimulator a(QStringLiteral("QTX"), 100.0, 80.0, 120.0, 7);
        PriceSimulator b(QStringLiteral("QTX"), 100.0, 80.0, 120.0, 7);

        for (int i = 0; i < 100; ++i) {
            QCOMPARE(a.next().price, b.next().price);
        }
    }

    void differentSeedsEventuallyDiverge()
    {
        PriceSimulator a(QStringLiteral("QTX"), 100.0, 80.0, 120.0, 1);
        PriceSimulator b(QStringLiteral("QTX"), 100.0, 80.0, 120.0, 2);

        bool diverged = false;
        for (int i = 0; i < 100; ++i) {
            if (!qFuzzyCompare(a.next().price + 1.0, b.next().price + 1.0)) {
                diverged = true;
                break;
            }
        }
        QVERIFY(diverged);
    }

    void currentPriceTracksTheLastSample()
    {
        PriceSimulator sim(QStringLiteral("QTX"), 100.0, 80.0, 120.0, 42);
        const PriceSimulator::Sample sample = sim.next();
        QCOMPARE(sim.currentPrice(), sample.price);
    }
};

QTEST_MAIN(TestPriceSimulator)
#include "test_price_simulator.moc"
