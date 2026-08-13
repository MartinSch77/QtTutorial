// SPDX-License-Identifier: MIT
#include "AlarmEvaluator.h"
#include "Historian.h"
#include "ProcessPointSimulator.h"
#include "TagTableModel.h"

#include <QTest>

using namespace qttutorial::plant_scada;

class TestPlantScadaLogic : public QObject {
    Q_OBJECT
private slots:
    void valueAtIsDeterministicForSameTime()
    {
        const TagDefinition tag = defaultTags().front();
        QCOMPARE(valueAt(tag, 12.5), valueAt(tag, 12.5));
    }

    void valueAtOscillatesWithinAmplitudeAroundBase()
    {
        const TagDefinition tag = defaultTags().front();
        for (double t = 0.0; t < tag.periodSeconds * 2.0; t += 5.0) {
            const double value = valueAt(tag, t);
            QVERIFY(value >= tag.baseValue - tag.amplitude - 1e-9);
            QVERIFY(value <= tag.baseValue + tag.amplitude + 1e-9);
        }
    }

    void evaluateClassifiesSeverityByLimits()
    {
        TagDefinition tag;
        tag.warnLow = 10.0;
        tag.warnHigh = 90.0;
        tag.criticalLow = 0.0;
        tag.criticalHigh = 100.0;

        QCOMPARE(evaluate(tag, 50.0), Severity::Normal);
        QCOMPARE(evaluate(tag, 95.0), Severity::Warning);
        QCOMPARE(evaluate(tag, 100.0), Severity::Critical);
        QCOMPARE(evaluate(tag, -1.0), Severity::Critical);
    }

    void historianRoundTripsInsertAndRangeQuery()
    {
        Historian historian(QStringLiteral("test_conn_roundtrip"));
        QVERIFY(historian.isOpen());
        QVERIFY(historian.createSchema());

        const QDateTime base = QDateTime::fromSecsSinceEpoch(1'700'000'000);
        for (int i = 0; i < 5; ++i) {
            QVERIFY(historian.insertSample(QStringLiteral("TAG.A"), base.addSecs(i * 10), 10.0 + i));
        }
        QVERIFY(historian.insertSample(QStringLiteral("TAG.B"), base, 999.0));

        const auto samples = historian.samplesInRange(QStringLiteral("TAG.A"), base, base.addSecs(40));
        QCOMPARE(samples.size(), std::size_t(5));
        QCOMPARE(samples.front().value, 10.0);
        QCOMPARE(samples.back().value, 14.0);

        const auto narrowed = historian.samplesInRange(QStringLiteral("TAG.A"), base.addSecs(15), base.addSecs(25));
        QCOMPARE(narrowed.size(), std::size_t(1));
        QCOMPARE(narrowed.front().value, 12.0);

        const auto latest = historian.latestSample(QStringLiteral("TAG.A"));
        QVERIFY(latest.has_value());
        QCOMPARE(latest->value, 14.0);
    }

    void historianReturnsEmptyForUnknownTag()
    {
        Historian historian(QStringLiteral("test_conn_empty"));
        QVERIFY(historian.createSchema());
        const auto samples = historian.samplesInRange(QStringLiteral("NOPE"), QDateTime::currentDateTime(),
                                                        QDateTime::currentDateTime().addSecs(60));
        QVERIFY(samples.empty());
        QVERIFY(!historian.latestSample(QStringLiteral("NOPE")).has_value());
    }

    void tagTableModelExposesUpdatedValuesAndStatus()
    {
        std::vector<TagDefinition> tags{defaultTags().front()};
        TagTableModel model(tags, nullptr);
        QCOMPARE(model.rowCount(), 1);

        model.updateValue(0, tags.front().baseValue);
        QCOMPARE(model.data(model.index(0, TagTableModel::StatusColumn), Qt::DisplayRole).toString(), QStringLiteral("Normal"));

        model.updateValue(0, tags.front().criticalHigh + 1.0);
        QCOMPARE(model.data(model.index(0, TagTableModel::StatusColumn), Qt::DisplayRole).toString(), QStringLiteral("Critical"));
    }
};

QTEST_MAIN(TestPlantScadaLogic)
#include "test_plant_scada_logic.moc"
