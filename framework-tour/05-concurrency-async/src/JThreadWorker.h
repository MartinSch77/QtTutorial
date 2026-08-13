// SPDX-License-Identifier: MIT
#pragma once

#include "SensorData.h"

#include <QObject>
#include <QString>

#include <atomic>
#include <stop_token>
#include <thread>
#include <vector>

namespace qttutorial::concurrency_async {

// The std::jthread side of the contrast: a plain background thread, cancelled
// cooperatively via std::stop_token, with no Qt Concurrent involved. JThreadWorker
// itself is a QObject that is never moved to another thread, so emitting its
// signals from the worker thread is safe: Qt's auto connection resolves to
// Qt::QueuedConnection because the receiver lives on a different thread than the
// one calling emit, and the signal/slot machinery is the thread-safe boundary --
// nothing here reaches into a QObject's internals directly from the worker thread.
class JThreadWorker : public QObject {
    Q_OBJECT
public:
    explicit JThreadWorker(QObject* parent = nullptr);
    ~JThreadWorker() override;

    void start(std::vector<SensorSample> samples, int channelCount);
    void requestStop();
    [[nodiscard]] bool isRunning() const;

signals:
    void progressChanged(int value, int max);
    void statusChanged(QString text);
    void finished(std::vector<ChannelStats> results, qint64 elapsedMs, std::size_t processedCount);
    void stopped();

private:
    void run(std::stop_token token, std::vector<SensorSample> samples, int channelCount);

    std::jthread m_thread;
    std::atomic_bool m_running{false};
};

} // namespace qttutorial::concurrency_async
