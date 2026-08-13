// SPDX-License-Identifier: MIT
#include "AlarmEvaluator.h"
#include "Historian.h"
#include "PlantOverviewModel.h"
#include "ProcessPointSimulator.h"
#include "TagTableModel.h"

#include <QTest>

#include <algorithm>
#include <cmath>

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

    void gatedTagFallsToResidualWhileItsLineIsStopped()
    {
        // MZ301.MOTOR_SPEED is gated: whenever Line 2 is stopped, the reading
        // should be near zero (a small residual) rather than continuing to
        // oscillate around its running baseline.
        const TagDefinition tag = defaultTags()[3];
        QCOMPARE(tag.tagId, QStringLiteral("MZ301.MOTOR_SPEED"));
        QVERIFY(tag.gatedByLineRunning);

        bool sawStopped = false;
        for (double t = 0.0; t < 480.0 && !sawStopped; t += 1.0) {
            if (!lineRunningAt(tag.line, t)) {
                sawStopped = true;
                const double value = valueAt(tag, t);
                QVERIFY(std::abs(value) < tag.baseValue * 0.1);
            }
        }
        QVERIFY(sawStopped);
    }

    void ungatedTagKeepsOscillatingRegardlessOfLineState()
    {
        // PT301.PRESSURE is not gated, so it should keep varying by roughly
        // its normal amplitude even while Line 2 happens to be stopped.
        const TagDefinition tag = defaultTags()[2];
        QCOMPARE(tag.tagId, QStringLiteral("PT301.PRESSURE"));
        QVERIFY(!tag.gatedByLineRunning);

        for (double t = 0.0; t < 480.0; t += 5.0) {
            const double value = valueAt(tag, t);
            QVERIFY(value >= tag.baseValue - tag.amplitude - 1e-9);
            QVERIFY(value <= tag.baseValue + tag.amplitude + 1e-9);
        }
    }

    void summarizeLinesGroupsByLineAndTracksRunningAndSeverity()
    {
        std::vector<TagDefinition> tags = defaultTags();
        // Force the Line 1 flow reading well above its running threshold and
        // above its critical limit, and leave everything else at baseline.
        std::vector<double> values;
        for (const TagDefinition& tag : tags) {
            values.push_back(tag.baseValue);
        }
        values[1] = tags[1].criticalHigh + 1.0; // FT201.FLOW, Line 1

        const auto lines = summarizeLines(tags, values);
        QCOMPARE(lines.size(), lineNames().size());

        const auto line1 = std::find_if(lines.begin(), lines.end(),
                                         [](const LineStatus& s) { return s.line == QStringLiteral("Line 1 - Filling"); });
        QVERIFY(line1 != lines.end());
        QVERIFY(line1->running);
        QCOMPARE(line1->worstSeverity, Severity::Critical);

        const auto line2 = std::find_if(lines.begin(), lines.end(),
                                         [](const LineStatus& s) { return s.line == QStringLiteral("Line 2 - Reaction"); });
        QVERIFY(line2 != lines.end());
        QCOMPARE(line2->worstSeverity, Severity::Normal);
    }

    void plantOverviewModelExposesLineRoles()
    {
        std::vector<TagDefinition> tags = defaultTags();
        std::vector<double> values;
        for (const TagDefinition& tag : tags) {
            values.push_back(tag.baseValue);
        }

        PlantOverviewModel model;
        model.update(tags, values);
        QCOMPARE(model.rowCount(), static_cast<int>(lineNames().size()));
        QVERIFY(!model.data(model.index(0, 0), PlantOverviewModel::LineNameRole).toString().isEmpty());
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
