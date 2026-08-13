// SPDX-License-Identifier: GPL-3.0-or-later
#include "WebSocketMarketFeed.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QWebSocket>
#include <QWebSocketServer>

namespace qttutorial::stock_tracker {

WebSocketMarketFeed::WebSocketMarketFeed(QObject* parent)
    : QObject(parent)
    , m_server(new QWebSocketServer(QStringLiteral("stock-tracker-feed"), QWebSocketServer::NonSecureMode, this))
    , m_client(new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this))
{
    connect(m_server, &QWebSocketServer::newConnection, this, &WebSocketMarketFeed::onServerNewConnection);
    connect(m_client, &QWebSocket::connected, this, &WebSocketMarketFeed::onClientConnected);
    connect(m_client, &QWebSocket::disconnected, this, &WebSocketMarketFeed::onClientDisconnected);
    connect(m_client, &QWebSocket::textMessageReceived, this, &WebSocketMarketFeed::onClientTextMessageReceived);
}

WebSocketMarketFeed::~WebSocketMarketFeed()
{
    if (m_client->state() == QAbstractSocket::ConnectedState) {
        m_client->close();
    }
    m_server->close();
}

bool WebSocketMarketFeed::start()
{
    if (!m_server->listen(QHostAddress::LocalHost)) {
        return false;
    }
    m_client->open(QUrl(QStringLiteral("ws://127.0.0.1:%1").arg(m_server->serverPort())));
    return true;
}

void WebSocketMarketFeed::publishTick(const QString& symbol, double price, qint64 sequence)
{
    if (m_serverSideSocket == nullptr) {
        return;
    }
    QJsonObject payload;
    payload[QStringLiteral("symbol")] = symbol;
    payload[QStringLiteral("price")] = price;
    payload[QStringLiteral("sequence")] = sequence;
    m_serverSideSocket->sendTextMessage(QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact)));
}

bool WebSocketMarketFeed::isConnected() const
{
    return m_connected;
}

quint16 WebSocketMarketFeed::serverPort() const
{
    return m_server->serverPort();
}

void WebSocketMarketFeed::onServerNewConnection()
{
    m_serverSideSocket = m_server->nextPendingConnection();
}

void WebSocketMarketFeed::onClientConnected()
{
    m_connected = true;
    emit connected();
}

void WebSocketMarketFeed::onClientDisconnected()
{
    m_connected = false;
    emit disconnected();
}

void WebSocketMarketFeed::onClientTextMessageReceived(const QString& message)
{
    const QJsonObject payload = QJsonDocument::fromJson(message.toUtf8()).object();
    emit tickReceived(payload.value(QStringLiteral("symbol")).toString(), payload.value(QStringLiteral("price")).toDouble(),
                       static_cast<qint64>(payload.value(QStringLiteral("sequence")).toDouble()));
}

} // namespace qttutorial::stock_tracker
