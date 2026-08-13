// SPDX-License-Identifier: MIT
#include "OrbitalSimulator.h"

#include <QTest>

using namespace qttutorial::space;

class TestOrbitalSimulator : public QObject {
    Q_OBJECT
private slots:
    void trueAnomalyAdvancesAndWraps()
    {
        OrbitalSimulator orbit;
        const double periodSeconds = orbit.state().orbitalPeriodMinutes * 60.0;
        orbit.step(periodSeconds * 0.25);
        QVERIFY(orbit.state().trueAnomalyDeg > 80.0 && orbit.state().trueAnomalyDeg < 100.0);

        orbit.step(periodSeconds);
        QVERIFY(orbit.state().trueAnomalyDeg >= 0.0 && orbit.state().trueAnomalyDeg < 360.0);
    }

    void entersEclipseOppositeTheSun()
    {
        OrbitalSimulator orbit(0.35);
        const double periodSeconds = orbit.state().orbitalPeriodMinutes * 60.0;
        orbit.step(periodSeconds * 0.5);
        QVERIFY(orbit.state().inEclipse);
        QCOMPARE(orbit.state().solarInputFraction, 0.0);
    }

    void isSunlitAtStart()
    {
        OrbitalSimulator orbit(0.35);
        orbit.step(0.001);
        QVERIFY(!orbit.state().inEclipse);
        QVERIFY(orbit.state().solarInputFraction > 0.0);
    }
};

QTEST_MAIN(TestOrbitalSimulator)
#include "test_orbital_simulator.moc"
