// SPDX-License-Identifier: MIT
#include "DeviceLink.h"

#include "FrameCodec.h"

#include <QIODevice>
#include <QTimer>

namespace qttutorial::serial_and_devices {

DeviceLink::DeviceLink(QIODevice* transport, int timeoutMs, QObject* parent)
    : QObject(parent)
    , m_transport(transport)
    , m_parser(this)
    , m_timer(new QTimer(this))
{
    m_timer->setSingleShot(true);
    m_timer->setInterval(timeoutMs);

    connect(m_transport, &QIODevice::readyRead, this, [this] {
        m_parser.feed(m_transport->readAll());
    });
    connect(&m_parser, &FrameParser::frameDecoded, this, [this](const QByteArray& payload) {
        m_timer->stop();
        emit frameReceived(payload);
    });
    connect(&m_parser, &FrameParser::frameError, this, &DeviceLink::frameRejected);
    connect(m_timer, &QTimer::timeout, this, &DeviceLink::timedOut);
}

void DeviceLink::sendFrame(const QByteArray& payload)
{
    m_transport->write(encodeFrame(payload));
    m_timer->start();
}

void DeviceLink::sendRaw(const QByteArray& rawBytes)
{
    m_transport->write(rawBytes);
    m_timer->start();
}

} // namespace qttutorial::serial_and_devices
