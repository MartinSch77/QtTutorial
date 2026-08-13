// SPDX-License-Identifier: MIT
#include "FleetOrbitSimulator.h"

#include <QTest>

#include <algorithm>

using namespace qttutorial::space;

class TestFleetOrbitSimulator : public QObject {
    Q_OBJECT
private slots:
    void seedsAFleetOfSatellites()
    {
        FleetOrbitSimulator sim;
        QVERIFY(!sim.satellites().empty());
        for (const auto& satellite : sim.satellites()) {
            QVERIFY(!satellite.name.isEmpty());
        }
    }

    void phaseAdvancesAndWraps()
    {
        FleetOrbitSimulator sim;
        const double initialPhase = sim.satellites().front().phaseDeg;
        for (int i = 0; i < 1000; ++i) {
            sim.step(60.0);
        }
        const double phase = sim.satellites().front().phaseDeg;
        QVERIFY(phase >= 0.0 && phase < 360.0);
        QVERIFY(phase != initialPhase);
    }

    void batteryDropsDuringSustainedEclipseAndRecoversInSun()
    {
        FleetOrbitSimulator sim;
        // Step through many short intervals to observe both eclipse and sunlit
        // periods over a couple of simulated orbits without skipping past them.
        double minBattery = 100.0;
        double maxBattery = 0.0;
        for (int i = 0; i < 20000; ++i) {
            sim.step(1.0);
            const double battery = sim.satellites().front().batteryPercent;
            minBattery = std::min(minBattery, battery);
            maxBattery = std::max(maxBattery, battery);
        }
        QVERIFY(maxBattery - minBattery > 1.0);
    }

    void healthIsConsistentWithBattery()
    {
        FleetOrbitSimulator sim;
        for (int i = 0; i < 100; ++i) {
            sim.step(30.0);
        }
        for (const auto& satellite : sim.satellites()) {
            QVERIFY(satellite.health == classifyHealth(satellite.batteryPercent, satellite.inEclipse));
        }
    }
};

QTEST_MAIN(TestFleetOrbitSimulator)
#include "test_fleet_orbit_simulator.moc"
