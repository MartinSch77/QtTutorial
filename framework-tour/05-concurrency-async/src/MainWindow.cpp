// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include <algorithm>

#include <QElapsedTimer>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QWidget>

namespace qttutorial::concurrency_async {

namespace {

QString formatStatsLine(const std::vector<ChannelStats>& results, qint64 elapsedMs, std::size_t sampleCount)
{
    const double seconds = std::max(1.0, static_cast<double>(elapsedMs)) / 1000.0;
    const double throughput = static_cast<double>(sampleCount) / seconds;

    QString text = QStringLiteral("elapsed %1 ms, %2 samples/sec\n").arg(elapsedMs).arg(throughput, 0, 'f', 0);
    for (const ChannelStats& stats : results) {
        text += QStringLiteral("ch%1: n=%2 min=%3 max=%4 mean=%5 std=%6\n")
                    .arg(stats.channel)
                    .arg(stats.sampleCount)
                    .arg(stats.min, 0, 'f', 2)
                    .arg(stats.max, 0, 'f', 2)
                    .arg(stats.mean, 0, 'f', 2)
                    .arg(stats.stddev, 0, 'f', 2);
    }
    return text;
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_qtConcurrentPipeline(new QtConcurrentPipeline(this))
    , m_jthreadWorker(new JThreadWorker(this))
    , m_qtConcurrentButton(new QPushButton(tr("Run via QtConcurrent"), this))
    , m_jthreadButton(new QPushButton(tr("Run via std::jthread"), this))
    , m_cancelButton(new QPushButton(tr("Cancel std::jthread run"), this))
    , m_qtConcurrentProgress(new QProgressBar(this))
    , m_jthreadProgress(new QProgressBar(this))
    , m_qtConcurrentStatsLabel(new QLabel(tr("No run yet."), this))
    , m_jthreadStatsLabel(new QLabel(tr("No run yet."), this))
    , m_jthreadStatusLabel(new QLabel(this))
    , m_samples(generateSamples(/*seed=*/42, /*sampleCount=*/60000, m_channelCount))
{
    setWindowTitle(tr("Concurrency & Async – QtConcurrent vs std::jthread"));

    m_qtConcurrentProgress->setRange(0, 100);
    m_jthreadProgress->setRange(0, 100);
    m_cancelButton->setEnabled(false);
    m_qtConcurrentStatsLabel->setWordWrap(true);
    m_jthreadStatsLabel->setWordWrap(true);

    auto* central = new QWidget(this);
    auto* layout = new QGridLayout(central);

    layout->addWidget(new QLabel(tr("<b>QtConcurrent::mapped + QFutureWatcher</b>"), this), 0, 0);
    layout->addWidget(m_qtConcurrentButton, 1, 0);
    layout->addWidget(m_qtConcurrentProgress, 2, 0);
    layout->addWidget(m_qtConcurrentStatsLabel, 3, 0);

    layout->addWidget(new QLabel(tr("<b>std::jthread + std::stop_token</b>"), this), 0, 1);
    layout->addWidget(m_jthreadButton, 1, 1);
    layout->addWidget(m_jthreadProgress, 2, 1);
    layout->addWidget(m_jthreadStatusLabel, 3, 1);
    layout->addWidget(m_jthreadStatsLabel, 4, 1);
    layout->addWidget(m_cancelButton, 5, 1);

    setCentralWidget(central);
    resize(720, 420);

    connect(m_qtConcurrentButton, &QPushButton::clicked, this, &MainWindow::onQtConcurrentClicked);
    connect(m_jthreadButton, &QPushButton::clicked, this, &MainWindow::onJThreadClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, [this] { m_jthreadWorker->requestStop(); });

    connect(m_qtConcurrentPipeline, &QtConcurrentPipeline::progressChanged, this,
            [this](int value, int max) { m_qtConcurrentProgress->setRange(0, max); m_qtConcurrentProgress->setValue(value); });
    connect(m_qtConcurrentPipeline, &QtConcurrentPipeline::finished, this,
            [this](const std::vector<ChannelStats>& results, qint64 elapsedMs, std::size_t sampleCount) {
                m_qtConcurrentStatsLabel->setText(formatStatsLine(results, elapsedMs, sampleCount));
                m_qtConcurrentButton->setEnabled(true);
            });

    connect(m_jthreadWorker, &JThreadWorker::progressChanged, this,
            [this](int value, int max) { m_jthreadProgress->setRange(0, max); m_jthreadProgress->setValue(value); });
    connect(m_jthreadWorker, &JThreadWorker::statusChanged, this,
            [this](const QString& text) { m_jthreadStatusLabel->setText(text); });
    connect(m_jthreadWorker, &JThreadWorker::finished, this, &MainWindow::onJThreadFinished);
    connect(m_jthreadWorker, &JThreadWorker::stopped, this, [this] {
        m_jthreadStatusLabel->setText(tr("Stopped by user (cooperative cancellation)."));
        m_jthreadButton->setEnabled(true);
        m_cancelButton->setEnabled(false);
    });
}

void MainWindow::onQtConcurrentClicked()
{
    m_qtConcurrentButton->setEnabled(false);
    m_qtConcurrentPipeline->start(m_samples, m_channelCount);
}

void MainWindow::onJThreadClicked()
{
    m_jthreadButton->setEnabled(false);
    m_cancelButton->setEnabled(true);
    m_jthreadWorker->start(m_samples, m_channelCount);
}

void MainWindow::onJThreadFinished(const std::vector<ChannelStats>& results, qint64 elapsedMs, std::size_t processedCount)
{
    m_jthreadStatsLabel->setText(formatStatsLine(results, elapsedMs, processedCount));
    m_jthreadButton->setEnabled(true);
    m_cancelButton->setEnabled(false);
}

} // namespace qttutorial::concurrency_async
