// SPDX-License-Identifier: MIT
#pragma once

#include "JThreadWorker.h"
#include "QtConcurrentPipeline.h"
#include "SensorData.h"

#include <QElapsedTimer>
#include <QMainWindow>

#include <vector>

class QLabel;
class QProgressBar;
class QPushButton;

namespace qttutorial::concurrency_async {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void onQtConcurrentClicked();
    void onJThreadClicked();
    void onJThreadFinished(const std::vector<ChannelStats>& results, qint64 elapsedMs, std::size_t processedCount);

    QtConcurrentPipeline* m_qtConcurrentPipeline;
    JThreadWorker* m_jthreadWorker;

    QPushButton* m_qtConcurrentButton;
    QPushButton* m_jthreadButton;
    QPushButton* m_cancelButton;

    QProgressBar* m_qtConcurrentProgress;
    QProgressBar* m_jthreadProgress;

    QLabel* m_qtConcurrentStatsLabel;
    QLabel* m_jthreadStatsLabel;
    QLabel* m_jthreadStatusLabel;

    std::vector<SensorSample> m_samples;
    int m_channelCount = 4;
};

} // namespace qttutorial::concurrency_async
