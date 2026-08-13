// SPDX-License-Identifier: MIT
#include "ShipState.h"

#include <QTest>

using namespace qttutorial::startrek_bridge;

class TestShipState : public QObject {
    Q_OBJECT
private slots:
    void startsAtGreenAlert()
    {
        ShipState state;
        QCOMPARE(state.alertLevel(), AlertLevel::Green);
        QCOMPARE(alertLevelName(state.alertLevel()), QStringLiteral("Green"));
    }

    void alertLevelTransitionsAreSticky()
    {
        ShipState state;
        state.setAlertLevel(AlertLevel::Yellow);
        QCOMPARE(state.alertLevel(), AlertLevel::Yellow);
        state.setAlertLevel(AlertLevel::Red);
        QCOMPARE(state.alertLevel(), AlertLevel::Red);
        state.setAlertLevel(AlertLevel::Green);
        QCOMPARE(state.alertLevel(), AlertLevel::Green);
    }

    void redAlertRaisesSharedEngineeringAndTacticalReadings()
    {
        // Setting Red alert once must be visible consistently through every
        // station's own accessor - not something each panel has to
        // separately recompute.
        ShipState greenState;
        for (int i = 0; i < 200; ++i) {
            greenState.advance(0.1);
        }
        const double greenPower = greenState.engineering().powerOutputPercent();
        const double greenShields = greenState.tactical().shieldStrengthPercent();

        ShipState redState;
        redState.setAlertLevel(AlertLevel::Red);
        for (int i = 0; i < 200; ++i) {
            redState.advance(0.1);
        }
        const double redPower = redState.engineering().powerOutputPercent();
        const double redShields = redState.tactical().shieldStrengthPercent();

        QVERIFY(redPower > greenPower);
        QVERIFY(redShields > greenShields);
    }

    void navigationWarpFactorStaysWithinPlausibleBounds()
    {
        ShipState state;
        state.setAlertLevel(AlertLevel::Red);
        for (int i = 0; i < 500; ++i) {
            state.advance(0.2);
            QVERIFY(state.navigation().warpFactor() >= 0.0);
            QVERIFY(state.navigation().warpFactor() <= 9.9);
        }
    }
};

QTEST_MAIN(TestShipState)
#include "test_ship_state.moc"
