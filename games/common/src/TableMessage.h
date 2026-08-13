// SPDX-License-Identifier: MIT
#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QMetaType>
#include <QString>

#include <expected>

namespace qttutorial::games::common {

// A generic envelope for one LAN table-game message: every card game and
// the kicker game reuse this same shape, distinguishing themselves purely
// by "type" and whatever they put in "payload" -- the transport layer
// (TableServer/TableClient) never needs to know a single game-specific
// field name.
struct TableMessage {
    QString type;
    int seat = -1; // -1 means "not yet seated" / server-assigned on connect
    QJsonObject payload;

    [[nodiscard]] QByteArray toLine() const;

    enum class DecodeError { InvalidJson, NotAnObject, MissingType };
    [[nodiscard]] static std::expected<TableMessage, DecodeError> fromLine(const QByteArray& line);
};

} // namespace qttutorial::games::common

Q_DECLARE_METATYPE(qttutorial::games::common::TableMessage)
