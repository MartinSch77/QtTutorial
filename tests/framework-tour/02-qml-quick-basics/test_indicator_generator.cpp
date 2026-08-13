// SPDX-License-Identifier: MIT
#include "IndicatorGenerator.h"

#include <QTest>

using namespace qttutorial::qml_basics;

class TestIndicatorGenerator : public QObject {
    Q_OBJECT
private slots:
    void generatesOneReadingPerIndicatorName()
    {
        const auto names = IndicatorGenerator::indicatorNames();
        const auto readings = IndicatorGenerator::generate(0);
        QCOMPARE(readings.size(), names.size());
        for (std::size_t i = 0; i < readings.size(); ++i) {
            QCOMPARE(readings[i].name, names[i]);
        }
    }

    void isDeterministicForTheSameTick()
    {
        const auto first = IndicatorGenerator::generate(7);
        const auto second = IndicatorGenerator::generate(7);
        QCOMPARE(first.size(), second.size());
        for (std::size_t i = 0; i < first.size(); ++i) {
            QCOMPARE(first[i].value, second[i].value);
        }
    }

    void keepsValuesWithinDisplayRange()
    {
        for (int tick = 0; tick < 50; ++tick) {
            for (const auto& reading : IndicatorGenerator::generate(tick)) {
                QVERIFY(reading.value >= 0.0);
                QVERIFY(reading.value <= 100.0);
            }
        }
    }

    void classifiesStatusByThreshold()
    {
        QCOMPARE(IndicatorGenerator::statusFor(10.0, 65.0, 85.0), IndicatorStatus::Nominal);
        QCOMPARE(IndicatorGenerator::statusFor(65.0, 65.0, 85.0), IndicatorStatus::Warning);
        QCOMPARE(IndicatorGenerator::statusFor(84.9, 65.0, 85.0), IndicatorStatus::Warning);
        QCOMPARE(IndicatorGenerator::statusFor(85.0, 65.0, 85.0), IndicatorStatus::Critical);
    }
};

QTEST_MAIN(TestIndicatorGenerator)
#include "test_indicator_generator.moc"
