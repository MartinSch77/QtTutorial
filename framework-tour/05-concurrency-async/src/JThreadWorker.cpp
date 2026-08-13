// SPDX-License-Identifier: MIT
#include "JThreadWorker.h"
#include "StatsCompute.h"

#include <QElapsedTimer>

#include <algorithm>
#include <chrono>
#include <format>
#include <ranges>
#include <span>

namespace qttutorial::concurrency_async {

JThreadWorker::JThreadWorker(QObject* parent)
    : QObject(parent)
{
}

JThreadWorker::~JThreadWorker()
{
    requestStop();
    // std::jthread's destructor also requests a stop and joins; this is explicit
    // for readability and so the object never outlives a running thread that
    // still touches captured state.
}

void JThreadWorker::start(std::vector<SensorSample> samples, int channelCount)
{
    requestStop();
    if (m_thread.joinable()) {
        m_thread.join();
    }
    m_thread = std::jthread([this, samples = std::move(samples), channelCount](std::stop_token token) {
        run(token, std::move(samples), channelCount);
    });
}

void JThreadWorker::requestStop()
{
    if (m_thread.get_stop_source().stop_possible()) {
        m_thread.request_stop();
    }
}

bool JThreadWorker::isRunning() const
{
    return m_running.load(std::memory_order_acquire);
}

void JThreadWorker::run(std::stop_token token, std::vector<SensorSample> samples, int channelCount)
{
    m_running.store(true, std::memory_order_release);

    QElapsedTimer timer;
    timer.start();

    const std::size_t channelSlots = static_cast<std::size_t>(std::max(channelCount, 1));
    std::vector<ChannelStatsAccumulator> accumulators(channelSlots);

    constexpr std::size_t batchSize = 200;
    std::size_t processed = 0;
    bool cancelled = false;

    for (std::size_t offset = 0; offset < samples.size(); offset += batchSize) {
        if (token.stop_requested()) {
            cancelled = true;
            break;
        }

        const std::size_t end = std::min(offset + batchSize, samples.size());
        const auto batch = std::span(samples).subspan(offset, end - offset);

        auto validSamples = batch | std::views::filter([channelSlots](const SensorSample& s) {
                                 return s.channel >= 0 && static_cast<std::size_t>(s.channel) < channelSlots;
                             });
        std::ranges::for_each(validSamples, [&accumulators](const SensorSample& sample) {
            accumulators[static_cast<std::size_t>(sample.channel)].add(sample.value);
        });
        processed = end;

        const int progress = static_cast<int>(processed * 100 / std::max<std::size_t>(samples.size(), 1));
        const std::string status =
            std::format("processed {} / {} samples ({} channels)", processed, samples.size(), channelCount);
        emit progressChanged(progress, 100);
        emit statusChanged(QString::fromStdString(status));

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    m_running.store(false, std::memory_order_release);

    if (cancelled) {
        emit stopped();
        return;
    }

    std::vector<ChannelStats> results;
    results.reserve(channelSlots);
    for (std::size_t channel = 0; channel < channelSlots; ++channel) {
        results.push_back(accumulators[channel].finish(static_cast<int>(channel)));
    }
    emit finished(std::move(results), timer.elapsed(), processed);
}

} // namespace qttutorial::concurrency_async
