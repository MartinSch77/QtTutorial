// SPDX-License-Identifier: MIT
#include "TacticalSimulator.h"

#include <QTest>

using namespace qttutorial::startrek_bridge;

class TestTacticalSimulator : public QObject {
    Q_OBJECT
private slots:
    void shieldsRampUpUnderRedAlertAndDownUnderGreen()
    {
        TacticalSimulator sim;
        for (int i = 0; i < 200; ++i) {
            sim.advance(0.1, AlertLevel::Red);
        }
        const double redShields = sim.shieldStrengthPercent();
        QVERIFY(redShields > 50.0);

        for (int i = 0; i < 200; ++i) {
            sim.advance(0.1, AlertLevel::Green);
        }
        QVERIFY(sim.shieldStrengthPercent() < redShields);
    }

    void readingsStayWithinPlausibleBoundsOverManyTicks()
    {
        TacticalSimulator sim;
        const AlertLevel levels[] = {AlertLevel::Green, AlertLevel::Yellow, AlertLevel::Red};
        for (int i = 0; i < 5000; ++i) {
            sim.advance(0.2, levels[i % 3]);
            QVERIFY(sim.externalRadiationLevel() >= 0.0);
            QVERIFY(sim.externalRadiationLevel() <= 5.0);
            QVERIFY(sim.hullStressLevel() >= 0.0);
            QVERIFY(sim.hullStressLevel() <= 100.0);
            QVERIFY(sim.shieldStrengthPercent() >= 0.0);
            QVERIFY(sim.shieldStrengthPercent() <= 100.0);
        }
    }

    void sensorContactsStayWithinTheScanRange()
    {
        TacticalSimulator sim;
        for (int i = 0; i < 20000; ++i) {
            sim.advance(0.25, AlertLevel::Yellow);
        }
        for (const SensorContact& contact : sim.contacts()) {
            QVERIFY(contact.distanceKm >= 0.0);
            QVERIFY(contact.distanceKm <= 200.0);
            QVERIFY(contact.bearingDeg >= 0.0);
            QVERIFY(contact.bearingDeg < 360.0);
        }
    }
};

QTEST_MAIN(TestTacticalSimulator)
#include "test_tactical_simulator.moc"
