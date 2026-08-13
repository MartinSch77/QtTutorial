// SPDX-License-Identifier: MIT
#include "ThermostatModel.h"

#include <QTest>

using namespace qttutorial::homeautomation;

class TestThermostatModel : public QObject {
    Q_OBJECT
private slots:
    void heatModeMovesTowardTargetAtExpectedRate()
    {
        ThermostatModel model;
        model.setCurrentTemperature(18.0);
        model.setTargetTemperature(22.0);
        model.setMode(ThermostatModel::Mode::Heat);

        model.advance(1.0);
        QCOMPARE(model.currentTemperature(), 18.0 + ThermostatModel::kHeatCoolRatePerSecond);
    }

    void heatModeDoesNotOvershootTarget()
    {
        ThermostatModel model;
        model.setCurrentTemperature(21.95);
        model.setTargetTemperature(22.0);
        model.setMode(ThermostatModel::Mode::Heat);

        model.advance(1.0);
        QCOMPARE(model.currentTemperature(), 22.0);
    }

    void heatModeHasNoEffectWhenAlreadyAboveTarget()
    {
        ThermostatModel model;
        model.setCurrentTemperature(25.0);
        model.setTargetTemperature(22.0);
        model.setMode(ThermostatModel::Mode::Heat);

        model.advance(5.0);
        QCOMPARE(model.currentTemperature(), 25.0);
    }

    void coolModeMovesTowardTargetDownward()
    {
        ThermostatModel model;
        model.setCurrentTemperature(26.0);
        model.setTargetTemperature(22.0);
        model.setMode(ThermostatModel::Mode::Cool);

        model.advance(1.0);
        QCOMPARE(model.currentTemperature(), 26.0 - ThermostatModel::kHeatCoolRatePerSecond);
    }

    void coolModeHasNoEffectWhenAlreadyBelowTarget()
    {
        ThermostatModel model;
        model.setCurrentTemperature(18.0);
        model.setTargetTemperature(22.0);
        model.setMode(ThermostatModel::Mode::Cool);

        model.advance(5.0);
        QCOMPARE(model.currentTemperature(), 18.0);
    }

    void offModeDriftsTowardOutsideAmbientRegardlessOfTarget()
    {
        ThermostatModel model;
        model.setCurrentTemperature(22.0);
        model.setTargetTemperature(22.0);
        model.setOutsideAmbient(8.0);
        model.setMode(ThermostatModel::Mode::Off);

        model.advance(1.0);
        QCOMPARE(model.currentTemperature(), 22.0 - ThermostatModel::kDriftRatePerSecond);
    }

    void offModeDriftIsSlowerThanActiveHeatCool()
    {
        QVERIFY(ThermostatModel::kDriftRatePerSecond < ThermostatModel::kHeatCoolRatePerSecond);
    }
};

QTEST_MAIN(TestThermostatModel)
#include "test_thermostat_model.moc"
