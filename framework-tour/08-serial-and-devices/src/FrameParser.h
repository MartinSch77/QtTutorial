// SPDX-License-Identifier: MIT
#pragma once

#include <QByteArray>
#include <QObject>

namespace qttutorial::serial_and_devices {

// Incremental decoder for the frame format described in FrameCodec.h. Real byte
// streams (serial or socket) can deliver a frame split across many reads, or several
// frames in one read, so feed() must be safe to call repeatedly with arbitrary chunk
// boundaries; it buffers internally and only emits frameDecoded() once a complete,
// checksum-valid frame has arrived.
class FrameParser : public QObject {
    Q_OBJECT
public:
    explicit FrameParser(QObject* parent = nullptr);

    void feed(const QByteArray& bytes);

signals:
    void frameDecoded(QByteArray payload);
    void frameError(QString reason);

private:
    QByteArray m_buffer;
};

} // namespace qttutorial::serial_and_devices
