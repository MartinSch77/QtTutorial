// SPDX-License-Identifier: MIT
#include "LanBeacon.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace qttutorial::games::common {

QByteArray LanBeacon::encode() const
{
    QJsonObject obj;
    obj[QStringLiteral("gameId")] = gameId;
    obj[QStringLiteral("hostName")] = hostName;
    obj[QStringLiteral("tcpPort")] = tcpPort;
    obj[QStringLiteral("seatsTaken")] = seatsTaken;
    obj[QStringLiteral("seatsTotal")] = seatsTotal;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

std::expected<LanBeacon, LanBeacon::DecodeError> LanBeacon::decode(const QByteArray& datagram)
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(datagram, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return std::unexpected(DecodeError::InvalidJson);
    }
    if (!doc.isObject()) {
        return std::unexpected(DecodeError::NotAnObject);
    }
    const QJsonObject obj = doc.object();
    if (!obj.contains(QStringLiteral("gameId")) || !obj.contains(QStringLiteral("tcpPort"))) {
        return std::unexpected(DecodeError::MissingField);
    }

    LanBeacon beacon;
    beacon.gameId = obj.value(QStringLiteral("gameId")).toString();
    beacon.hostName = obj.value(QStringLiteral("hostName")).toString();
    beacon.tcpPort = static_cast<quint16>(obj.value(QStringLiteral("tcpPort")).toInt());
    beacon.seatsTaken = obj.value(QStringLiteral("seatsTaken")).toInt();
    beacon.seatsTotal = obj.value(QStringLiteral("seatsTotal")).toInt();
    return beacon;
}

} // namespace qttutorial::games::common
