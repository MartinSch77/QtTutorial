// SPDX-License-Identifier: MIT
#include "Satellite.h"

#include <QTest>

using namespace qttutorial::space;

class TestSatellite : public QObject {
    Q_OBJECT
private slots:
    void nominalWhenBatteryHealthyAndSunlit()
    {
        QVERIFY(classifyHealth(90.0, false) == HealthSummary::Nominal);
    }

    void cautionWhenBatteryLowButNotCritical()
    {
        QVERIFY(classifyHealth(25.0, false) == HealthSummary::Caution);
    }

    void cautionWhenModeratelyLowAndInEclipse()
    {
        QVERIFY(classifyHealth(35.0, true) == HealthSummary::Caution);
        QVERIFY(classifyHealth(35.0, false) == HealthSummary::Nominal);
    }

    void criticalWhenBatteryVeryLow()
    {
        QVERIFY(classifyHealth(10.0, false) == HealthSummary::Critical);
        QVERIFY(classifyHealth(10.0, true) == HealthSummary::Critical);
    }

    void toStringProducesExpectedLabels()
    {
        QCOMPARE(toString(HealthSummary::Nominal), QStringLiteral("Nominal"));
        QCOMPARE(toString(HealthSummary::Caution), QStringLiteral("Caution"));
        QCOMPARE(toString(HealthSummary::Critical), QStringLiteral("Critical"));
    }
};

QTEST_MAIN(TestSatellite)
#include "test_satellite.moc"
