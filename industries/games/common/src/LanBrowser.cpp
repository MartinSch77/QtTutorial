// SPDX-License-Identifier: MIT
#include "LanBrowser.h"

#include <QDateTime>
#include <QNetworkDatagram>

namespace qttutorial::games::common {

namespace {
constexpr qint64 kStaleAfterMs = 5000;
}

LanBrowser::LanBrowser(QObject* parent)
    : QObject(parent)
    // Value members must not be given `this` as a QObject parent -- see
    // TableServer.cpp for why that crashes on destruction.
    , m_socket()
    , m_expiryTimer()
{
    connect(&m_socket, &QUdpSocket::readyRead, this, &LanBrowser::onReadyRead);
    connect(&m_expiryTimer, &QTimer::timeout, this, &LanBrowser::expireStaleEntries);
}

bool LanBrowser::start()
{
    if (!m_socket.bind(QHostAddress::AnyIPv4, kLanDiscoveryPort, QUdpSocket::ShareAddress)) {
        return false;
    }
    m_expiryTimer.start(1000);
    return true;
}

void LanBrowser::stop()
{
    m_socket.close();
    m_expiryTimer.stop();
}

QList<DiscoveredGame> LanBrowser::discoveredGames(const QString& gameId) const
{
    QList<DiscoveredGame> games;
    for (auto it = m_entries.cbegin(); it != m_entries.cend(); ++it) {
        if (!gameId.isEmpty() && it.value().beacon.gameId != gameId) {
            continue;
        }
        const qsizetype colonIndex = it.key().lastIndexOf(':');
        games.append({it.key().left(colonIndex), it.value().beacon});
    }
    return games;
}

void LanBrowser::onReadyRead()
{
    bool changed = false;
    while (m_socket.hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_socket.receiveDatagram();
        const auto beacon = LanBeacon::decode(datagram.data());
        if (!beacon.has_value()) {
            continue;
        }
        const QString key = datagram.senderAddress().toString() + QLatin1Char(':')
            + QString::number(beacon->tcpPort);
        m_entries[key] = {*beacon, QDateTime::currentMSecsSinceEpoch()};
        changed = true;
    }
    if (changed) {
        emit gamesChanged();
    }
}

void LanBrowser::expireStaleEntries()
{
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    bool changed = false;
    for (auto it = m_entries.begin(); it != m_entries.end();) {
        if (now - it.value().lastSeenMs > kStaleAfterMs) {
            it = m_entries.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }
    if (changed) {
        emit gamesChanged();
    }
}

} // namespace qttutorial::games::common
