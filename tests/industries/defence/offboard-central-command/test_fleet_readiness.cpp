// SPDX-License-Identifier: MIT
#include "FleetReadiness.h"

#include <QTest>

using namespace qttutorial::defence;

class TestFleetReadiness : public QObject {
    Q_OBJECT
private slots:
    void emptyFleetSummarizesToZero()
    {
        const FleetReadinessSummary summary = summarizeReadiness({});
        QCOMPARE(summary.nominal, 0);
        QCOMPARE(summary.caution, 0);
        QCOMPARE(summary.critical, 0);
        QCOMPARE(summary.total(), 0);
    }

    void countsEachHealthBandSeparately()
    {
        std::vector<Asset> assets;
        Asset a;
        a.health = QStringLiteral("Nominal");
        assets.push_back(a);
        assets.push_back(a);
        Asset b;
        b.health = QStringLiteral("Caution");
        assets.push_back(b);
        Asset c;
        c.health = QStringLiteral("Critical");
        assets.push_back(c);
        assets.push_back(c);
        assets.push_back(c);

        const FleetReadinessSummary summary = summarizeReadiness(assets);
        QCOMPARE(summary.nominal, 2);
        QCOMPARE(summary.caution, 1);
        QCOMPARE(summary.critical, 3);
        QCOMPARE(summary.total(), 6);
    }

    void unrecognisedHealthStringCountsAsNominal()
    {
        std::vector<Asset> assets;
        Asset a;
        a.health = QStringLiteral("SomethingElse");
        assets.push_back(a);

        const FleetReadinessSummary summary = summarizeReadiness(assets);
        QCOMPARE(summary.nominal, 1);
        QCOMPARE(summary.caution, 0);
        QCOMPARE(summary.critical, 0);
    }
};

QTEST_MAIN(TestFleetReadiness)
#include "test_fleet_readiness.moc"
