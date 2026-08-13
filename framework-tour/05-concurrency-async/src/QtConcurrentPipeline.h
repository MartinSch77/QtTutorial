// SPDX-License-Identifier: MIT
#pragma once

#include "SensorData.h"

#include <QElapsedTimer>
#include <QFutureWatcher>
#include <QObject>

#include <vector>

namespace qttutorial::concurrency_async {

// Runs computeChannelStats() for every channel on the Qt Concurrent thread pool via
// QtConcurrent::mapped, and surfaces progress/results through QFutureWatcher signals.
// This is the "Qt-native" side of the contrast: no explicit threads, no manual
// cross-thread signal marshalling to write, the watcher already lives on this
// object's thread so its signals arrive queued automatically.
class QtConcurrentPipeline : public QObject {
    Q_OBJECT
public:
    explicit QtConcurrentPipeline(QObject* parent = nullptr);

    void start(std::vector<SensorSample> samples, int channelCount);
    void cancel();
    [[nodiscard]] bool isRunning() const;

signals:
    void progressChanged(int value, int max);
    void finished(std::vector<ChannelStats> results, qint64 elapsedMs, std::size_t sampleCount);

private:
    QFutureWatcher<ChannelStats> m_watcher;
    QElapsedTimer m_timer;
    std::size_t m_sampleCount = 0;
};

} // namespace qttutorial::concurrency_async
