// SPDX-License-Identifier: MIT
#include "MainWindow.h"

#include "Protocol.h"

#include <QDateTime>
#include <QHostAddress>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTcpSocket>
#include <QVBoxLayout>
#include <QWidget>

namespace qttutorial::networking {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_server(new EchoServer(this))
    , m_clientSocket(new QTcpSocket(this))
    , m_sendButton(new QPushButton(tr("Send request"), this))
    , m_log(new QPlainTextEdit(this))
    , m_latencyLabel(new QLabel(tr("No requests sent yet"), this))
{
    setWindowTitle(tr("Qt Network – JSON Echo Demo"));

    m_log->setReadOnly(true);

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->addWidget(m_sendButton);
    layout->addWidget(m_log);
    layout->addWidget(m_latencyLabel);
    setCentralWidget(central);

    m_server->listen(QHostAddress::LocalHost);
    appendLog(tr("Server listening on 127.0.0.1:%1").arg(m_server->serverPort()));

    connect(m_clientSocket, &QTcpSocket::readyRead, this, &MainWindow::handleSocketReadyRead);
    connect(m_clientSocket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        appendLog(tr("Client socket error: %1").arg(m_clientSocket->errorString()));
    });
    connect(m_clientSocket, &QTcpSocket::connected, this, [this] {
        appendLog(tr("Client connected to server"));
    });

    m_clientSocket->connectToHost(QHostAddress::LocalHost, m_server->serverPort());

    connect(m_sendButton, &QPushButton::clicked, this, &MainWindow::sendRequest);

    resize(520, 420);
}

void MainWindow::sendRequest()
{
    if (m_clientSocket->state() != QAbstractSocket::ConnectedState) {
        appendLog(tr("Cannot send request: client is not connected"));
        return;
    }

    const int id = m_nextRequestId++;
    Request request;
    request.id = id;
    request.clientTimestampMs = QDateTime::currentMSecsSinceEpoch();

    QElapsedTimer timer;
    timer.start();
    m_pendingRequests.insert(id, timer);

    m_clientSocket->write(encodeRequest(request));
    appendLog(tr("-> request id=%1").arg(id));
}

void MainWindow::handleSocketReadyRead()
{
    m_receiveBuffer += m_clientSocket->readAll();

    qsizetype newlineIndex = 0;
    while ((newlineIndex = m_receiveBuffer.indexOf('\n')) != -1) {
        const QByteArray line = m_receiveBuffer.left(newlineIndex);
        m_receiveBuffer.remove(0, newlineIndex + 1);

        const std::optional<Response> response = decodeResponse(line);
        if (!response) {
            appendLog(tr("<- received malformed response"));
            continue;
        }

        const auto it = m_pendingRequests.find(response->id);
        if (it == m_pendingRequests.end()) {
            appendLog(tr("<- response for unknown id=%1").arg(response->id));
            continue;
        }

        const qint64 latencyMs = it->elapsed();
        m_pendingRequests.erase(it);

        appendLog(tr("<- response id=%1 echo=\"%2\" latency=%3 ms")
                       .arg(response->id)
                       .arg(response->echo)
                       .arg(latencyMs));
        updateLatencyLabel(latencyMs);
    }
}

void MainWindow::appendLog(const QString& line)
{
    m_log->appendPlainText(line);
}

void MainWindow::updateLatencyLabel(qint64 latestLatencyMs)
{
    m_totalLatencyMs += latestLatencyMs;
    ++m_completedRequestCount;
    const double averageMs = static_cast<double>(m_totalLatencyMs) / m_completedRequestCount;

    m_latencyLabel->setText(tr("Last latency: %1 ms · Average over %2 request(s): %3 ms")
                                 .arg(latestLatencyMs)
                                 .arg(m_completedRequestCount)
                                 .arg(averageMs, 0, 'f', 2));
}

} // namespace qttutorial::networking
