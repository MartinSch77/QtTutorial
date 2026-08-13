// SPDX-License-Identifier: MIT
#include "NavigationSimulator.h"

#include <QTest>

#include <cmath>

using namespace qttutorial::startrek_bridge;

class TestNavigationSimulator : public QObject {
    Q_OBJECT
private slots:
    void headingStaysNormalizedOverManyTicks()
    {
        NavigationSimulator sim(50.0);
        for (int i = 0; i < 5000; ++i) {
            sim.advance(0.2, AlertLevel::Green);
            QVERIFY(sim.headingDeg() >= 0.0);
            QVERIFY(sim.headingDeg() < 360.0);
        }
    }

    void warpAndImpulseStayWithinPlausibleBoundsUnderAllAlertLevels()
    {
        NavigationSimulator sim(50.0);
        const AlertLevel levels[] = {AlertLevel::Green, AlertLevel::Yellow, AlertLevel::Red};
        for (int i = 0; i < 3000; ++i) {
            sim.advance(0.15, levels[i % 3]);
            QVERIFY(sim.warpFactor() >= 0.0);
            QVERIFY(sim.warpFactor() <= 9.9);
            QVERIFY(sim.impulsePercent() >= 0.0);
            QVERIFY(sim.impulsePercent() <= 100.0);
        }
    }

    void contactsStayWithinTheLocalSpaceArea()
    {
        const double areaRadiusKm = 50.0;
        NavigationSimulator sim(areaRadiusKm);
        for (int i = 0; i < 20000; ++i) {
            sim.advance(0.25, AlertLevel::Red);
        }
        for (const NavContact& contact : sim.contacts()) {
            const double distance = std::hypot(contact.xKm, contact.yKm);
            QVERIFY(distance <= areaRadiusKm + 1e-6);
        }
    }
};

QTEST_MAIN(TestNavigationSimulator)
#include "test_navigation_simulator.moc"
