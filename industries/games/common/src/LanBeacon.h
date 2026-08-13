// SPDX-License-Identifier: MIT
#pragma once

#include <QByteArray>
#include <QString>

#include <expected>

namespace qttutorial::games::common {

// One "there is a game here" announcement, broadcast periodically by
// LanAdvertiser and collected by LanBrowser. Kept as a small, pure,
// independently-testable encode/decode pair -- the actual QUdpSocket
// plumbing around it is a thin, hard-to-unit-test shell by comparison.
struct LanBeacon {
    QString gameId;   // e.g. "schafkopf", "kicker" -- distinguishes game types on the same LAN
    QString hostName; // human-readable, shown in a "join game" list
    quint16 tcpPort = 0;
    int seatsTaken = 0;
    int seatsTotal = 0;

    [[nodiscard]] QByteArray encode() const;

    enum class DecodeError { InvalidJson, NotAnObject, MissingField };
    [[nodiscard]] static std::expected<LanBeacon, DecodeError> decode(const QByteArray& datagram);
};

} // namespace qttutorial::games::common
