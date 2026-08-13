// SPDX-License-Identifier: MIT
#include "Protocol.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace qttutorial::networking {

namespace {
// Each encoded message is terminated with '\n' so the receiving side can
// split the raw socket stream back into individual JSON objects.
QByteArray frame(const QJsonObject& object)
{
    return QJsonDocument(object).toJson(QJsonDocument::Compact) + '\n';
}
} // namespace

QByteArray encodeRequest(const Request& request)
{
    QJsonObject object;
    object[QStringLiteral("id")] = request.id;
    object[QStringLiteral("clientTimestampMs")] = request.clientTimestampMs;
    return frame(object);
}

std::optional<Request> decodeRequest(const QByteArray& line)
{
    const QJsonDocument doc = QJsonDocument::fromJson(line);
    if (!doc.isObject()) {
        return std::nullopt;
    }
    const QJsonObject object = doc.object();
    if (!object.contains(QStringLiteral("id")) || !object.contains(QStringLiteral("clientTimestampMs"))) {
        return std::nullopt;
    }

    Request request;
    request.id = object.value(QStringLiteral("id")).toInt();
    request.clientTimestampMs = static_cast<qint64>(object.value(QStringLiteral("clientTimestampMs")).toDouble());
    return request;
}

QByteArray encodeResponse(const Response& response)
{
    QJsonObject object;
    object[QStringLiteral("id")] = response.id;
    object[QStringLiteral("clientTimestampMs")] = response.clientTimestampMs;
    object[QStringLiteral("serverTimestampMs")] = response.serverTimestampMs;
    object[QStringLiteral("echo")] = response.echo;
    return frame(object);
}

std::optional<Response> decodeResponse(const QByteArray& line)
{
    const QJsonDocument doc = QJsonDocument::fromJson(line);
    if (!doc.isObject()) {
        return std::nullopt;
    }
    const QJsonObject object = doc.object();
    if (!object.contains(QStringLiteral("id")) || !object.contains(QStringLiteral("clientTimestampMs"))
        || !object.contains(QStringLiteral("serverTimestampMs"))) {
        return std::nullopt;
    }

    Response response;
    response.id = object.value(QStringLiteral("id")).toInt();
    response.clientTimestampMs = static_cast<qint64>(object.value(QStringLiteral("clientTimestampMs")).toDouble());
    response.serverTimestampMs = static_cast<qint64>(object.value(QStringLiteral("serverTimestampMs")).toDouble());
    response.echo = object.value(QStringLiteral("echo")).toString();
    return response;
}

} // namespace qttutorial::networking
