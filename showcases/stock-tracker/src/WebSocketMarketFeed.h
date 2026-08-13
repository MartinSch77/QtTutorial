// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

// This header is only compiled into stock_tracker_lib when Qt6::WebSockets was
// found (Qt >= 6.5); see showcases/stock-tracker/CMakeLists.txt. It is never
// included otherwise.

#include <QObject>
#include <QString>

class QWebSocketServer;
class QWebSocket;

namespace qttutorial::stock_tracker {

// Wraps a REAL local WebSocket round trip over loopback: a QWebSocketServer
// bound to 127.0.0.1 on an ephemeral port, and a QWebSocket client that
// connects to that same server. publishTick() sends a small JSON text frame
// from the server side; tickReceived() is emitted from the client side only
// once that frame has actually been received back over the socket. This is
// not a timer dressed up as "streaming" - every tick genuinely goes out over a
// socket and back.
class WebSocketMarketFeed : public QObject {
    Q_OBJECT
public:
    explicit WebSocketMarketFeed(QObject* parent = nullptr);
    ~WebSocketMarketFeed() override;

    // Starts the local server on an ephemeral port and connects the client to
    // it. Returns true once the server is listening (the client connection
    // itself completes asynchronously; wait for connected()).
    bool start();

    void publishTick(const QString& symbol, double price, qint64 sequence);

    [[nodiscard]] bool isConnected() const;
    [[nodiscard]] quint16 serverPort() const;

signals:
    void connected();
    void disconnected();
    void tickReceived(QString symbol, double price, qint64 sequence);

private slots:
    void onServerNewConnection();
    void onClientConnected();
    void onClientDisconnected();
    void onClientTextMessageReceived(const QString& message);

private:
    QWebSocketServer* m_server;
    QWebSocket* m_client;
    QWebSocket* m_serverSideSocket = nullptr;
    bool m_connected = false;
};

} // namespace qttutorial::stock_tracker
