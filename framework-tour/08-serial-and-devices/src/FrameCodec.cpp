// SPDX-License-Identifier: MIT
#include "FrameCodec.h"

namespace qttutorial::serial_and_devices {

quint8 computeChecksum(const QByteArray& payload)
{
    quint8 sum = 0;
    for (const char byte : payload) {
        sum = static_cast<quint8>(sum ^ static_cast<unsigned char>(byte));
    }
    return sum;
}

QByteArray encodeFrame(const QByteArray& payload)
{
    if (payload.size() > kMaxPayloadSize) {
        return {};
    }

    QByteArray frame;
    frame.reserve(payload.size() + 4);
    frame.append(static_cast<char>(kFrameStx));
    frame.append(static_cast<char>(static_cast<unsigned char>(payload.size())));
    frame.append(payload);
    frame.append(static_cast<char>(computeChecksum(payload)));
    frame.append(static_cast<char>(kFrameEtx));
    return frame;
}

} // namespace qttutorial::serial_and_devices
