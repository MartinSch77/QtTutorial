// SPDX-License-Identifier: MIT
#include "FlightDynamics.h"

#include <QTest>

using namespace qttutorial::avionics;

class TestFlightDynamics : public QObject {
    Q_OBJECT
private slots:
    void staysLevelWithNoInput()
    {
        FlightDynamics dynamics;
        for (int i = 0; i < 100; ++i) {
            dynamics.step(0.1);
        }
        QVERIFY(qFuzzyIsNull(dynamics.state().pitchDeg));
        QVERIFY(qFuzzyIsNull(dynamics.state().rollDeg));
    }

    void aileronInputProducesRollAndTurn()
    {
        FlightDynamics dynamics;
        ControlInput input;
        input.aileron = 1.0;
        dynamics.setControlInput(input);
        for (int i = 0; i < 200; ++i) {
            dynamics.step(0.05);
        }
        QVERIFY(dynamics.state().rollDeg > 20.0);

        const double headingAfterRoll = dynamics.state().headingDeg;
        for (int i = 0; i < 50; ++i) {
            dynamics.step(0.05);
        }
        QVERIFY(dynamics.state().headingDeg != headingAfterRoll);
    }

    void elevatorInputProducesClimb()
    {
        FlightDynamics dynamics;
        ControlInput input;
        input.elevator = 1.0;
        input.throttle = 0.8;
        dynamics.setControlInput(input);
        const double startAltitude = dynamics.state().altitudeFt;
        for (int i = 0; i < 300; ++i) {
            dynamics.step(0.05);
        }
        QVERIFY(dynamics.state().pitchDeg > 5.0);
        QVERIFY(dynamics.state().verticalSpeedFtPerMin > 0.0);
        QVERIFY(dynamics.state().altitudeFt > startAltitude);
    }

    void climbingBleedsAirspeed()
    {
        FlightDynamics level;
        ControlInput levelInput;
        levelInput.throttle = 0.5;
        level.setControlInput(levelInput);

        FlightDynamics climbing;
        ControlInput climbInput;
        climbInput.throttle = 0.5;
        climbInput.elevator = 1.0;
        climbing.setControlInput(climbInput);

        for (int i = 0; i < 400; ++i) {
            level.step(0.05);
            climbing.step(0.05);
        }
        QVERIFY(climbing.state().airspeedKt < level.state().airspeedKt);
    }

    void sustainedHighThrottleClimbRaisesEngineTemp()
    {
        FlightDynamics cruise;
        ControlInput cruiseInput;
        cruiseInput.throttle = 0.3;
        cruise.setControlInput(cruiseInput);

        FlightDynamics climbing;
        ControlInput climbInput;
        climbInput.throttle = 1.0;
        climbInput.elevator = 1.0;
        climbing.setControlInput(climbInput);

        for (int i = 0; i < 400; ++i) {
            cruise.step(0.05);
            climbing.step(0.05);
        }
        QVERIFY(climbing.state().engineTempC > cruise.state().engineTempC);
        QVERIFY(climbing.state().engineTempC > FlightDynamics::kEngineTempNominalC);
    }

    void ignoresNonPositiveTimeStep()
    {
        FlightDynamics dynamics;
        ControlInput input;
        input.elevator = 1.0;
        dynamics.setControlInput(input);
        dynamics.step(1.0);
        const FlightState before = dynamics.state();
        dynamics.step(0.0);
        dynamics.step(-1.0);
        const FlightState after = dynamics.state();
        QCOMPARE(after.pitchDeg, before.pitchDeg);
        QCOMPARE(after.altitudeFt, before.altitudeFt);
    }
};

QTEST_MAIN(TestFlightDynamics)
#include "test_flight_dynamics.moc"
