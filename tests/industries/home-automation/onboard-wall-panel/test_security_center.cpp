// SPDX-License-Identifier: MIT
#include "SecurityCenter.h"

#include <QTest>

using namespace qttutorial::homeautomation;

class TestSecurityCenter : public QObject {
    Q_OBJECT
private slots:
    void notArmedIsNeverABreachEvenWithTriggeredSensor()
    {
        SecurityCenter center;
        center.setArmed(false);
        center.setSensorTriggered(0, true);
        QVERIFY(!center.isBreach());
    }

    void armedWithTriggeredSensorIsABreach()
    {
        SecurityCenter center;
        center.setArmed(true);
        center.setSensorTriggered(0, true);
        QVERIFY(center.isBreach());
    }

    void armedWithNoTriggeredSensorsIsNotABreach()
    {
        SecurityCenter center;
        center.setArmed(true);
        QVERIFY(!center.isBreach());
    }

    void clearingTheOnlyTriggeredSensorEndsTheBreach()
    {
        SecurityCenter center;
        center.setArmed(true);
        center.setSensorTriggered(0, true);
        QVERIFY(center.isBreach());
        center.setSensorTriggered(0, false);
        QVERIFY(!center.isBreach());
    }
};

QTEST_MAIN(TestSecurityCenter)
#include "test_security_center.moc"
