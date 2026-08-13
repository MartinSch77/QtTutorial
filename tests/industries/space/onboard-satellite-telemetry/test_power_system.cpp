// SPDX-License-Identifier: MIT
#include "PowerSystem.h"

#include <QTest>

using namespace qttutorial::space;

class TestPowerSystem : public QObject {
    Q_OBJECT
private slots:
    void chargesWhenSunlitAndPointed()
    {
        PowerSystem power;
        const double before = power.state().batterySocPercent;
        for (int i = 0; i < 50; ++i) {
            power.step(60.0, 1.0, true);
        }
        QVERIFY(power.state().batterySocPercent > before);
    }

    void discharesInEclipseEvenWhenPointed()
    {
        PowerSystem power;
        const double before = power.state().batterySocPercent;
        for (int i = 0; i < 50; ++i) {
            power.step(60.0, 0.0, true);
        }
        QVERIFY(power.state().batterySocPercent < before);
        QCOMPARE(power.state().solarPanelOutputWatts, 0.0);
    }

    void doesNotChargeWhenNotSunPointedEvenInSun()
    {
        PowerSystem power;
        const double before = power.state().batterySocPercent;
        for (int i = 0; i < 50; ++i) {
            power.step(60.0, 1.0, false);
        }
        QVERIFY(power.state().batterySocPercent < before);
        QCOMPARE(power.state().solarPanelOutputWatts, 0.0);
    }

    void socStaysWithinBounds()
    {
        PowerSystem power;
        for (int i = 0; i < 10000; ++i) {
            power.step(60.0, 1.0, true);
        }
        QVERIFY(power.state().batterySocPercent <= 100.0);
        QVERIFY(power.state().batterySocPercent >= 0.0);
    }
};

QTEST_MAIN(TestPowerSystem)
#include "test_power_system.moc"
