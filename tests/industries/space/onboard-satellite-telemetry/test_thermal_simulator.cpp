// SPDX-License-Identifier: MIT
#include "ThermalSimulator.h"

#include <QTest>

using namespace qttutorial::space;

class TestThermalSimulator : public QObject {
    Q_OBJECT
private slots:
    void warmsTowardHotSideInSun()
    {
        ThermalSimulator thermal;
        const double before = thermal.temperatureC(ThermalZone::Radiator);
        for (int i = 0; i < 200; ++i) {
            thermal.step(10.0, 1.0);
        }
        QVERIFY(thermal.temperatureC(ThermalZone::Radiator) > before);
    }

    void coolsTowardColdSideInEclipse()
    {
        ThermalSimulator thermal;
        const double before = thermal.temperatureC(ThermalZone::Radiator);
        for (int i = 0; i < 200; ++i) {
            thermal.step(10.0, 0.0);
        }
        QVERIFY(thermal.temperatureC(ThermalZone::Radiator) < before);
    }

    void allZonesRespondIndependently()
    {
        ThermalSimulator thermal;
        for (int i = 0; i < 500; ++i) {
            thermal.step(10.0, 1.0);
        }
        QVERIFY(thermal.temperatureC(ThermalZone::Radiator) != thermal.temperatureC(ThermalZone::BatteryBay));
    }
};

QTEST_MAIN(TestThermalSimulator)
#include "test_thermal_simulator.moc"
