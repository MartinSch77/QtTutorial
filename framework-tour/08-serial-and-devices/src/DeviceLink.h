// SPDX-License-Identifier: MIT
#pragma once

#include "FrameParser.h"

#include <QObject>

class QIODevice;
class QTimer;

namespace qttutorial::serial_and_devices {

// Sends framed messages over any QIODevice (a SimulatedSerialLink endpoint today, a
// real QSerialPort if physical hardware is present later) and decodes framed replies
// coming back the other way. sendFrame() arms a timeout: if no complete, valid frame
// arrives within timeoutMs, timedOut() is emitted instead of hanging forever, which
// is the behavior any real serial protocol needs to survive an unplugged cable or an
// unresponsive device.
class DeviceLink : public QObject {
    Q_OBJECT
public:
    explicit DeviceLink(QIODevice* transport, int timeoutMs = 500, QObject* parent = nullptr);

    void sendFrame(const QByteArray& payload);

    // Writes already-framed bytes verbatim and arms the timeout, bypassing
    // encodeFrame(). Intended for deliberately sending a malformed/corrupted frame
    // (e.g. from a UI "test error handling" button) to demonstrate that the remote
    // FrameParser rejects it and, since no valid reply follows, that this link's own
    // timeout still fires.
    void sendRaw(const QByteArray& rawBytes);

signals:
    void frameReceived(QByteArray payload);
    void frameRejected(QString reason);
    void timedOut();

private:
    QIODevice* m_transport;
    FrameParser m_parser;
    QTimer* m_timer;
};

} // namespace qttutorial::serial_and_devices
