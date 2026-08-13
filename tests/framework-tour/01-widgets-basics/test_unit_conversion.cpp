// SPDX-License-Identifier: MIT
#include "UnitConversion.h"

#include <QTest>

using namespace qttutorial::units;

class TestUnitConversion : public QObject {
    Q_OBJECT
private slots:
    void convertsLengthAcrossUnits()
    {
        const auto result = convert(1.0, "km", "m");
        QVERIFY(result.has_value());
        QCOMPARE(*result, 1000.0);
    }

    void convertsCelsiusToFahrenheit()
    {
        const auto result = convert(0.0, "C", "F");
        QVERIFY(result.has_value());
        QVERIFY(qFuzzyCompare(*result, 32.0));
    }

    void convertsFahrenheitToCelsiusRoundTrip()
    {
        const auto toF = convert(37.0, "C", "F");
        QVERIFY(toF.has_value());
        const auto backToC = convert(*toF, "F", "C");
        QVERIFY(backToC.has_value());
        QVERIFY(qFuzzyCompare(*backToC, 37.0));
    }

    void rejectsIncompatibleQuantities()
    {
        const auto result = convert(1.0, "kg", "m");
        QVERIFY(!result.has_value());
        QCOMPARE(result.error(), ConversionError::IncompatibleQuantities);
    }

    void rejectsUnknownUnit()
    {
        const auto result = convert(1.0, "parsec", "m");
        QVERIFY(!result.has_value());
        QCOMPARE(result.error(), ConversionError::UnknownUnit);
    }
};

QTEST_MAIN(TestUnitConversion)
#include "test_unit_conversion.moc"
