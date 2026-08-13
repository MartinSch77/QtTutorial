// SPDX-License-Identifier: MIT
#include "EnergyMonitor.h"

#include <QTest>

using namespace qttutorial::homeautomation::remote;

class TestEnergyMonitor : public QObject {
    Q_OBJECT
private slots:
    void allLightsOffAndThermostatOffIsJustBaseLoad()
    {
        Snapshot snapshot;
        snapshot.rooms = {RoomState{.name = QStringLiteral("Living Room"), .lightOn = false, .brightness = 0}};
        snapshot.thermostatMode = 0;
        QCOMPARE(EnergyMonitor::estimateWatts(snapshot), EnergyMonitor::kBaseLoadWatts);
    }

    void eachLightOnAtFullBrightnessAddsItsFullShare()
    {
        Snapshot snapshot;
        snapshot.rooms = {
            RoomState{.name = QStringLiteral("A"), .lightOn = true, .brightness = 100},
            RoomState{.name = QStringLiteral("B"), .lightOn = true, .brightness = 100},
        };
        snapshot.thermostatMode = 0;
        const double expected = EnergyMonitor::kBaseLoadWatts + 2 * EnergyMonitor::kWattsPerLightAtFullBrightness;
        QCOMPARE(EnergyMonitor::estimateWatts(snapshot), expected);
    }

    void dimmedLightContributesProportionallyLessThanFullBrightness()
    {
        Snapshot dim;
        dim.rooms = {RoomState{.name = QStringLiteral("A"), .lightOn = true, .brightness = 20}};
        Snapshot bright;
        bright.rooms = {RoomState{.name = QStringLiteral("A"), .lightOn = true, .brightness = 100}};
        QVERIFY(EnergyMonitor::estimateWatts(dim) < EnergyMonitor::estimateWatts(bright));
    }

    void activeHeatingOrCoolingAddsAFlatLoadRegardlessOfMode()
    {
        Snapshot off;
        off.thermostatMode = 0;
        Snapshot heat;
        heat.thermostatMode = 1;
        Snapshot cool;
        cool.thermostatMode = 2;

        QCOMPARE(EnergyMonitor::estimateWatts(heat), EnergyMonitor::estimateWatts(off) + EnergyMonitor::kHeatCoolWatts);
        QCOMPARE(EnergyMonitor::estimateWatts(cool), EnergyMonitor::estimateWatts(off) + EnergyMonitor::kHeatCoolWatts);
    }

    void awayLikeSnapshotDrawsLessThanEverythingOnSnapshot()
    {
        Snapshot away;
        away.rooms = {RoomState{.name = QStringLiteral("A"), .lightOn = false, .brightness = 0}};
        away.thermostatMode = 0;

        Snapshot everythingOn;
        everythingOn.rooms = {RoomState{.name = QStringLiteral("A"), .lightOn = true, .brightness = 100}};
        everythingOn.thermostatMode = 1;

        QVERIFY(EnergyMonitor::estimateWatts(away) < EnergyMonitor::estimateWatts(everythingOn));
    }

    void historyDropsOldestSampleOnceCapacityIsExceeded()
    {
        EnergyHistory history(3);
        history.addSample(100.0, 1);
        history.addSample(200.0, 2);
        history.addSample(300.0, 3);
        history.addSample(400.0, 4);

        QCOMPARE(history.samples().size(), std::size_t{3});
        QCOMPARE(history.samples().front().watts, 200.0);
        QCOMPARE(history.samples().back().watts, 400.0);
    }

    void historyAverageAndPeakAreComputedOverCurrentSamples()
    {
        EnergyHistory history(10);
        history.addSample(100.0, 1);
        history.addSample(300.0, 2);

        QCOMPARE(history.averageWatts(), 200.0);
        QCOMPARE(history.peakWatts(), 300.0);
    }

    void emptyHistoryHasZeroAverageAndPeak()
    {
        EnergyHistory history(10);
        QCOMPARE(history.averageWatts(), 0.0);
        QCOMPARE(history.peakWatts(), 0.0);
    }
};

QTEST_MAIN(TestEnergyMonitor)
#include "test_energy_monitor.moc"
