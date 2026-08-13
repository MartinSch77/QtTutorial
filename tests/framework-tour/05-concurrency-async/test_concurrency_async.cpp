// SPDX-License-Identifier: MIT
#include "JThreadWorker.h"
#include "QtConcurrentPipeline.h"
#include "SensorData.h"
#include "StatsCompute.h"

#include <QSignalSpy>
#include <QTest>

#include <cmath>

using namespace qttutorial::concurrency_async;

class TestConcurrencyAsync : public QObject {
    Q_OBJECT
private slots:
    void sampleGenerationIsDeterministic()
    {
        const auto a = generateSamples(/*seed=*/7, /*sampleCount=*/500, /*channelCount=*/3);
        const auto b = generateSamples(/*seed=*/7, /*sampleCount=*/500, /*channelCount=*/3);

        QCOMPARE(a.size(), b.size());
        for (std::size_t i = 0; i < a.size(); ++i) {
            QCOMPARE(a[i].timestampMs, b[i].timestampMs);
            QCOMPARE(a[i].channel, b[i].channel);
            QCOMPARE(a[i].value, b[i].value);
        }
    }

    void sampleGenerationDiffersAcrossSeeds()
    {
        const auto a = generateSamples(1, 500, 3);
        const auto b = generateSamples(2, 500, 3);

        bool anyDifferent = false;
        for (std::size_t i = 0; i < a.size(); ++i) {
            if (!qFuzzyCompare(a[i].value, b[i].value)) {
                anyDifferent = true;
                break;
            }
        }
        QVERIFY(anyDifferent);
    }

    void computeChannelStatsMatchesKnownInput()
    {
        const std::vector<SensorSample> samples{
            {.timestampMs = 0, .channel = 0, .value = 1.0},
            {.timestampMs = 10, .channel = 0, .value = 2.0},
            {.timestampMs = 20, .channel = 0, .value = 3.0},
            {.timestampMs = 30, .channel = 1, .value = 100.0},
        };

        const ChannelStats stats = computeChannelStats(samples, 0);
        QCOMPARE(stats.sampleCount, std::size_t{3});
        QVERIFY(qFuzzyCompare(stats.min, 1.0));
        QVERIFY(qFuzzyCompare(stats.max, 3.0));
        QVERIFY(qFuzzyCompare(stats.mean, 2.0));
        QVERIFY(qAbs(stats.stddev - std::sqrt(2.0 / 3.0)) < 1e-9);
    }

    void computeAllChannelStatsCoversEveryChannel()
    {
        const auto samples = generateSamples(1, 1000, 4);
        const auto results = computeAllChannelStats(samples, 4);

        QCOMPARE(results.size(), std::size_t{4});
        std::size_t total = 0;
        for (int channel = 0; channel < 4; ++channel) {
            QCOMPARE(results[static_cast<std::size_t>(channel)].channel, channel);
            total += results[static_cast<std::size_t>(channel)].sampleCount;
        }
        QCOMPARE(total, samples.size());
    }

    void qtConcurrentPipelineProducesResults()
    {
        const auto samples = generateSamples(5, 2000, 3);
        const auto expected = computeAllChannelStats(samples, 3);

        QtConcurrentPipeline pipeline;
        QSignalSpy finishedSpy(&pipeline, &QtConcurrentPipeline::finished);
        pipeline.start(samples, 3);

        QVERIFY(finishedSpy.wait(5000));
        QCOMPARE(finishedSpy.count(), 1);

        const auto results = finishedSpy.at(0).at(0).value<std::vector<ChannelStats>>();
        QCOMPARE(results.size(), expected.size());
        for (std::size_t i = 0; i < results.size(); ++i) {
            QCOMPARE(results[i].channel, expected[i].channel);
            QCOMPARE(results[i].sampleCount, expected[i].sampleCount);
            QVERIFY(qFuzzyCompare(results[i].mean, expected[i].mean));
        }
    }

    void jthreadWorkerStopsPromptlyOnRequest()
    {
        JThreadWorker worker;
        bool stopped = false;
        bool finished = false;
        connect(&worker, &JThreadWorker::stopped, [&] { stopped = true; });
        connect(&worker, &JThreadWorker::finished, [&](std::vector<ChannelStats>, qint64, std::size_t) {
            finished = true;
        });

        const auto samples = generateSamples(3, 2'000'000, 4);
        worker.start(samples, 4);

        QTRY_VERIFY(worker.isRunning());
        worker.requestStop();

        QTRY_VERIFY_WITH_TIMEOUT(!worker.isRunning(), 3000);
        QTRY_VERIFY(stopped);
        QVERIFY(!finished);
    }
};

QTEST_MAIN(TestConcurrencyAsync)
#include "test_concurrency_async.moc"
