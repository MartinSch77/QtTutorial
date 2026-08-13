// SPDX-License-Identifier: MIT
#include "QtConcurrentPipeline.h"
#include "StatsCompute.h"

#include <QtConcurrent/QtConcurrent>

#include <algorithm>

namespace qttutorial::concurrency_async {

QtConcurrentPipeline::QtConcurrentPipeline(QObject* parent)
    : QObject(parent)
{
    connect(&m_watcher, &QFutureWatcher<ChannelStats>::progressValueChanged, this,
            [this](int value) { emit progressChanged(value, m_watcher.progressMaximum()); });

    connect(&m_watcher, &QFutureWatcher<ChannelStats>::finished, this, [this] {
        const QList<ChannelStats> resultList = m_watcher.future().results();
        std::vector<ChannelStats> results(resultList.begin(), resultList.end());
        std::ranges::sort(results, {}, &ChannelStats::channel);
        emit finished(std::move(results), m_timer.elapsed(), m_sampleCount);
    });
}

void QtConcurrentPipeline::start(std::vector<SensorSample> samples, int channelCount)
{
    m_sampleCount = samples.size();
    m_timer.start();

    std::vector<int> channels(static_cast<std::size_t>(std::max(channelCount, 0)));
    for (std::size_t i = 0; i < channels.size(); ++i) {
        channels[i] = static_cast<int>(i);
    }

    auto samplesShared = std::make_shared<std::vector<SensorSample>>(std::move(samples));
    QFuture<ChannelStats> future = QtConcurrent::mapped(channels, [samplesShared](int channel) {
        return computeChannelStats(*samplesShared, channel);
    });
    m_watcher.setFuture(future);
}

void QtConcurrentPipeline::cancel()
{
    m_watcher.cancel();
}

bool QtConcurrentPipeline::isRunning() const
{
    return m_watcher.isRunning();
}

} // namespace qttutorial::concurrency_async
