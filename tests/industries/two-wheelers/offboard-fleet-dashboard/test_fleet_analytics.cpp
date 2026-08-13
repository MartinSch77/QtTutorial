// SPDX-License-Identifier: MIT
#include "FleetAnalytics.h"

#include <QTest>

using namespace qttutorial::two_wheelers::fleet;

class TestFleetAnalytics : public QObject {
    Q_OBJECT
private slots:
    void utilizationPercentIsZeroForEmptyFleet()
    {
        QCOMPARE(utilizationPercent({}), 0.0);
    }

    void utilizationPercentCountsOnlyRidingVehicles()
    {
        std::vector<VehicleSample> samples(4);
        samples[0].status = RiderStatus::Riding;
        samples[1].status = RiderStatus::Riding;
        samples[2].status = RiderStatus::Idle;
        samples[3].status = RiderStatus::Charging;

        QCOMPARE(utilizationPercent(samples), 50.0);
    }

    void utilizationHistoryStartsEmpty()
    {
        FleetUtilizationHistory history(5);
        QCOMPARE(history.size(), std::size_t{0});
        QVERIFY(history.values().empty());
    }

    void utilizationHistoryBoundsToCapacity()
    {
        FleetUtilizationHistory history(3);
        history.addSample(10.0);
        history.addSample(20.0);
        history.addSample(30.0);
        history.addSample(40.0);

        QCOMPARE(history.size(), std::size_t{3});
        const std::vector<double> values = history.values();
        QCOMPARE(values.size(), std::size_t{3});
        QCOMPARE(values[0], 20.0);
        QCOMPARE(values[1], 30.0);
        QCOMPARE(values[2], 40.0);
    }

    void utilizationHistoryPreservesInsertionOrder()
    {
        FleetUtilizationHistory history(10);
        history.addSample(1.0);
        history.addSample(2.0);
        history.addSample(3.0);

        const std::vector<double> values = history.values();
        QCOMPARE(values.size(), std::size_t{3});
        QCOMPARE(values[0], 1.0);
        QCOMPARE(values[2], 3.0);
    }
};

QTEST_MAIN(TestFleetAnalytics)
#include "test_fleet_analytics.moc"
