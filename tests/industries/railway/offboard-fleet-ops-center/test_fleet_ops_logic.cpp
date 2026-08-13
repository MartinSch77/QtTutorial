// SPDX-License-Identifier: MIT
#include "DelayCalculator.h"
#include "FleetMapGeometry.h"
#include "FleetSimulator.h"
#include "FleetTableModel.h"
#include "RunHistoryStore.h"

#include <QTest>

#include <cmath>

using namespace qttutorial::fleet_ops;

class TestFleetOpsLogic : public QObject {
    Q_OBJECT
private slots:
    void classifyDelayBucketsCorrectly()
    {
        QCOMPARE(classifyDelay(0.0), Punctuality::OnTime);
        QCOMPARE(classifyDelay(-2.0), Punctuality::Early);
        QCOMPARE(classifyDelay(3.0), Punctuality::MinorDelay);
        QCOMPARE(classifyDelay(10.0), Punctuality::MajorDelay);
    }

    void trainStatePositionIsConsistentWithElapsedTimeAndSpeed()
    {
        FleetConfig config;
        config.trainCount = 3;
        config.loopLengthKm = 60.0;
        config.baseSpeedKmh = 90.0;

        const TrainState atStart = trainStateAt(config, 0, 0.0);
        const TrainState afterOneHour = trainStateAt(config, 0, 3600.0);

        QCOMPARE(atStart.positionKm, 0.0);
        QCOMPARE(afterOneHour.positionKm, 30.0);
    }

    void trainsAreEvenlySpacedAtStartOfSimulation()
    {
        FleetConfig config;
        config.trainCount = 4;
        config.loopLengthKm = 40.0;

        const std::vector<TrainState> fleet = fleetStateAt(config, 0.0);
        QCOMPARE(fleet.size(), std::size_t(4));
        for (std::size_t i = 0; i < fleet.size(); ++i) {
            QCOMPARE(fleet[i].positionKm, 10.0 * static_cast<double>(i));
        }
    }

    void distanceToNextStopIsNeverNegativeOrLargerThanSpacing()
    {
        FleetConfig config;
        for (double t = 0.0; t < 5000.0; t += 137.0) {
            const TrainState state = trainStateAt(config, 0, t);
            QVERIFY(state.distanceToNextStopKm >= 0.0);
            QVERIFY(state.distanceToNextStopKm <= config.stationSpacingKm);
        }
    }

    void positionOnLoopPlacesStartAtTopAndSweepsClockwise()
    {
        const MapPoint start = positionOnLoop(0.0, 40.0, 100.0);
        QVERIFY(std::abs(start.x) < 1e-9);
        QVERIFY(start.y < 0.0); // top of the circle

        const MapPoint quarter = positionOnLoop(10.0, 40.0, 100.0);
        QVERIFY(quarter.x > 0.0); // a quarter of the way round is to the right
        QVERIFY(std::abs(quarter.y) < 1.0);

        const MapPoint half = positionOnLoop(20.0, 40.0, 100.0);
        QVERIFY(std::abs(half.x) < 1e-9);
        QVERIFY(half.y > 0.0); // bottom of the circle
    }

    void positionOnLoopWrapsPastTheLoopLength()
    {
        const MapPoint atZero = positionOnLoop(0.0, 40.0, 100.0);
        const MapPoint wrapped = positionOnLoop(40.0, 40.0, 100.0);
        QVERIFY(std::abs(atZero.x - wrapped.x) < 1e-6);
        QVERIFY(std::abs(atZero.y - wrapped.y) < 1e-6);
    }

    void positionOnLoopStaysOnTheCircleRegardlessOfPosition()
    {
        for (double positionKm = 0.0; positionKm < 100.0; positionKm += 7.0) {
            const MapPoint point = positionOnLoop(positionKm, 40.0, 100.0);
            const double radius = std::sqrt(point.x * point.x + point.y * point.y);
            QVERIFY(std::abs(radius - 100.0) < 1e-6);
        }
    }

    void runHistoryStoreRoundTripsAndAveragesDelay()
    {
        RunHistoryStore store(QStringLiteral("test_conn_fleet_roundtrip"));
        QVERIFY(store.isOpen());
        QVERIFY(store.createSchema());

        const QDateTime base = QDateTime::fromSecsSinceEpoch(1'700'000'000);
        RunSample sampleA{base, 1.0, 90.0, 2.0};
        RunSample sampleB{base.addSecs(60), 1.5, 88.0, 4.0};
        QVERIFY(store.insertSample(QStringLiteral("T-01"), sampleA));
        QVERIFY(store.insertSample(QStringLiteral("T-01"), sampleB));

        const auto samples = store.samplesInRange(QStringLiteral("T-01"), base, base.addSecs(120));
        QCOMPARE(samples.size(), std::size_t(2));
        QCOMPARE(samples.front().delayMinutes, 2.0);
        QCOMPARE(samples.back().delayMinutes, 4.0);

        const double avg = store.averageDelayInRange(base, base.addSecs(120));
        QCOMPARE(avg, 3.0);
    }

    void runHistoryStoreNetworkDelaySeriesBucketsAcrossTrains()
    {
        RunHistoryStore store(QStringLiteral("test_conn_fleet_network_series"));
        QVERIFY(store.isOpen());
        QVERIFY(store.createSchema());

        const QDateTime base = QDateTime::fromSecsSinceEpoch(1'700'000'000);
        // Two trains, two samples each, both trains in the same first
        // 30-second bucket and both in the same second bucket, so the
        // network series should average across trains within each bucket.
        QVERIFY(store.insertSample(QStringLiteral("T-01"), RunSample{base, 0.0, 90.0, 2.0}));
        QVERIFY(store.insertSample(QStringLiteral("T-02"), RunSample{base.addSecs(5), 0.0, 90.0, 6.0}));
        QVERIFY(store.insertSample(QStringLiteral("T-01"), RunSample{base.addSecs(40), 0.0, 90.0, 10.0}));
        QVERIFY(store.insertSample(QStringLiteral("T-02"), RunSample{base.addSecs(45), 0.0, 90.0, 20.0}));

        const auto series = store.networkDelaySeries(base, base.addSecs(120), 30);
        QCOMPARE(series.size(), std::size_t(2));
        QCOMPARE(series[0].second, 4.0);  // average of 2.0 and 6.0
        QCOMPARE(series[1].second, 15.0); // average of 10.0 and 20.0
        QVERIFY(series[0].first < series[1].first);
    }

    void fleetTableModelReflectsUpdatedStates()
    {
        FleetTableModel model;
        FleetConfig config;
        config.trainCount = 2;
        model.setStates(fleetStateAt(config, 0.0));

        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(model.data(model.index(0, FleetTableModel::TrainIdColumn), Qt::DisplayRole).toString(),
                 QStringLiteral("T-01"));
    }
};

QTEST_MAIN(TestFleetOpsLogic)
#include "test_fleet_ops_logic.moc"
