// SPDX-License-Identifier: MIT
#include "TableMessage.h"

#include <QJsonDocument>

namespace qttutorial::games::common {

QByteArray TableMessage::toLine() const
{
    QJsonObject obj;
    obj[QStringLiteral("type")] = type;
    obj[QStringLiteral("seat")] = seat;
    obj[QStringLiteral("payload")] = payload;
    return QJsonDocument(obj).toJson(QJsonDocument::Compact) + '\n';
}

std::expected<TableMessage, TableMessage::DecodeError> TableMessage::fromLine(const QByteArray& line)
{
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return std::unexpected(DecodeError::InvalidJson);
    }
    if (!doc.isObject()) {
        return std::unexpected(DecodeError::NotAnObject);
    }
    const QJsonObject obj = doc.object();
    if (!obj.contains(QStringLiteral("type")) || !obj.value(QStringLiteral("type")).isString()) {
        return std::unexpected(DecodeError::MissingType);
    }

    TableMessage message;
    message.type = obj.value(QStringLiteral("type")).toString();
    message.seat = obj.value(QStringLiteral("seat")).toInt(-1);
    message.payload = obj.value(QStringLiteral("payload")).toObject();
    return message;
}

} // namespace qttutorial::games::common
