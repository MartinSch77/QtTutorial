// SPDX-License-Identifier: MIT
#include "AnnunciatorLogic.h"
#include "FlightDynamics.h"

#include <QTest>

using namespace qttutorial::avionics;

namespace {

bool hasMessage(const std::vector<CautionMessage>& messages, const std::string& id)
{
    for (const auto& message : messages) {
        if (message.id == id) {
            return true;
        }
    }
    return false;
}

} // namespace

class TestAnnunciatorLogic : public QObject {
    Q_OBJECT
private slots:
    void nominalStateProducesNoCautions()
    {
        AnnunciatorLogic logic;
        FlightState state;
        const auto messages = logic.evaluate(state);
        QVERIFY(messages.empty());
    }

    void excessiveBankTriggersBankAngleWarning()
    {
        AnnunciatorLogic logic;
        FlightState state;
        state.rollDeg = 35.0;
        const auto messages = logic.evaluate(state);
        QVERIFY(hasMessage(messages, "BANK ANGLE"));
    }

    void lowAirspeedTriggersLowSpeedWarning()
    {
        AnnunciatorLogic logic;
        FlightState state;
        state.airspeedKt = 80.0;
        const auto messages = logic.evaluate(state);
        QVERIFY(hasMessage(messages, "LOW SPEED"));
    }

    void highSinkRateNearGroundTriggersSinkRateWarning()
    {
        AnnunciatorLogic logic;
        FlightState state;
        state.verticalSpeedFtPerMin = -2500.0;
        state.altitudeFt = 1200.0;
        const auto messages = logic.evaluate(state);
        QVERIFY(hasMessage(messages, "SINK RATE"));
    }

    void highSinkRateAtCruiseAltitudeDoesNotTrigger()
    {
        AnnunciatorLogic logic;
        FlightState state;
        state.verticalSpeedFtPerMin = -2500.0;
        state.altitudeFt = 20000.0;
        const auto messages = logic.evaluate(state);
        QVERIFY(!hasMessage(messages, "SINK RATE"));
    }

    void hotEngineTriggersEngineCaution()
    {
        AnnunciatorLogic logic;
        FlightState state;
        state.engineTempC = FlightDynamics::kEngineTempCautionC + 10.0;
        const auto messages = logic.evaluate(state);
        QVERIFY(hasMessage(messages, "ENGINE"));
    }

    void sustainedHighThrottleClimbCorrelatesWithEngineCaution()
    {
        // The caution is a *consequence* of the coupled dynamics model, not an
        // independently scripted flag: drive a sustained high-power climb
        // through FlightDynamics and confirm the resulting engine temperature
        // is what the annunciator reacts to.
        FlightDynamics dynamics;
        ControlInput input;
        input.elevator = 1.0;
        input.throttle = 1.0;
        dynamics.setControlInput(input);
        for (int i = 0; i < 600; ++i) {
            dynamics.step(0.05);
        }
        QVERIFY(dynamics.state().engineTempC > FlightDynamics::kEngineTempNominalC);

        AnnunciatorLogic logic;
        const auto messages = logic.evaluate(dynamics.state());
        QVERIFY(hasMessage(messages, "ENGINE"));
    }
};

QTEST_MAIN(TestAnnunciatorLogic)
#include "test_annunciator_logic.moc"
