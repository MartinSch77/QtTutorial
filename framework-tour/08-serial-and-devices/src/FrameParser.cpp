// SPDX-License-Identifier: MIT
#include "FrameParser.h"

#include "FrameCodec.h"

namespace qttutorial::serial_and_devices {

FrameParser::FrameParser(QObject* parent)
    : QObject(parent)
{
}

void FrameParser::feed(const QByteArray& bytes)
{
    m_buffer.append(bytes);

    while (true) {
        const qsizetype stxIndex = m_buffer.indexOf(static_cast<char>(kFrameStx));
        if (stxIndex < 0) {
            // No frame start in the buffer at all: whatever we have is noise.
            m_buffer.clear();
            return;
        }
        if (stxIndex > 0) {
            // Drop leading garbage that precedes the next STX.
            m_buffer.remove(0, stxIndex);
        }

        if (m_buffer.size() < 2) {
            return; // Waiting for the length byte to arrive.
        }

        const auto len = static_cast<quint8>(m_buffer.at(1));
        const qsizetype frameSize = 2 + len + 1 + 1; // STX + LEN + payload + CHECKSUM + ETX

        if (m_buffer.size() < frameSize) {
            return; // Incomplete frame: wait for more bytes, buffer is left as-is.
        }

        const QByteArray payload = m_buffer.mid(2, len);
        const auto receivedChecksum = static_cast<quint8>(m_buffer.at(2 + len));
        const auto etxByte = static_cast<unsigned char>(m_buffer.at(2 + len + 1));

        m_buffer.remove(0, frameSize);

        if (etxByte != kFrameEtx) {
            emit frameError(QStringLiteral("Frame discarded: missing ETX terminator"));
            continue;
        }

        const quint8 expectedChecksum = computeChecksum(payload);
        if (receivedChecksum != expectedChecksum) {
            emit frameError(QStringLiteral("Frame discarded: checksum mismatch (expected 0x%1, got 0x%2)")
                                 .arg(expectedChecksum, 2, 16, QLatin1Char('0'))
                                 .arg(receivedChecksum, 2, 16, QLatin1Char('0')));
            continue;
        }

        emit frameDecoded(payload);
    }
}

} // namespace qttutorial::serial_and_devices
