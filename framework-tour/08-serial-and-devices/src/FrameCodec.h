// SPDX-License-Identifier: MIT
#pragma once

#include <QByteArray>

namespace qttutorial::serial_and_devices {

// Frame layout used by every message exchanged over a device link, whether that
// link is the real Qt6::SerialPort or the SimulatedSerialLink below:
//
//   [STX][LEN][PAYLOAD x LEN][CHECKSUM][ETX]
//
// STX/ETX are fixed marker bytes, LEN is a single byte (0-255) giving the payload
// size, and CHECKSUM is the XOR-fold of the payload bytes. This is intentionally a
// simple, real, testable framing scheme, not a placeholder.
inline constexpr unsigned char kFrameStx = 0x02;
inline constexpr unsigned char kFrameEtx = 0x03;
inline constexpr int kMaxPayloadSize = 255;

quint8 computeChecksum(const QByteArray& payload);

// Returns an empty QByteArray if payload exceeds kMaxPayloadSize (the 1-byte LEN
// field cannot represent it).
QByteArray encodeFrame(const QByteArray& payload);

} // namespace qttutorial::serial_and_devices
