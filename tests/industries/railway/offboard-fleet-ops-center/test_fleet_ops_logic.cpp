// SPDX-License-Identifier: MIT
#include "DelayCalculator.h"
#include "FleetSimulator.h"
#include "FleetTableModel.h"
#include "RunHistoryStore.h"

#include <QTest>

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
