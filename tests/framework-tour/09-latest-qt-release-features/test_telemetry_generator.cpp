// SPDX-License-Identifier: GPL-3.0-or-later
#include "TelemetryGenerator.h"

#include <QTest>

using namespace qttutorial::graphs_showcase;

class TestTelemetryGenerator : public QObject {
    Q_OBJECT
private slots:
    void reportsThreeNamedSensors()
    {
        const auto names = TelemetryGenerator::sensorNames();
        QCOMPARE(names.size(), std::size_t{3});
        for (const auto& name : names) {
            QVERIFY(!name.isEmpty());
        }
    }

    void isDeterministicForTheSameElapsedTime()
    {
        for (int sensor = 0; sensor < 3; ++sensor) {
            const double first = TelemetryGenerator::valueAt(sensor, 4.2);
            const double second = TelemetryGenerator::valueAt(sensor, 4.2);
            QCOMPARE(first, second);
        }
    }

    void returnsZeroForOutOfRangeSensorIndex()
    {
        QCOMPARE(TelemetryGenerator::valueAt(-1, 1.0), 0.0);
        QCOMPARE(TelemetryGenerator::valueAt(3, 1.0), 0.0);
    }
};

QTEST_MAIN(TestTelemetryGenerator)
#include "test_telemetry_generator.moc"
