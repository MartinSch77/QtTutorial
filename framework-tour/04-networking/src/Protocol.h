// SPDX-License-Identifier: MIT
#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

namespace qttutorial::networking {

// One JSON object per line ("newline-delimited JSON"): simple to frame over a
// raw QTcpSocket byte stream without a length prefix, and easy to eyeball
// while debugging with e.g. `nc` or a log view.
struct Request {
    int id = 0;
    qint64 clientTimestampMs = 0;
};

struct Response {
    int id = 0;
    qint64 clientTimestampMs = 0;
    qint64 serverTimestampMs = 0;
    QString echo;
};

[[nodiscard]] QByteArray encodeRequest(const Request& request);
[[nodiscard]] std::optional<Request> decodeRequest(const QByteArray& line);

[[nodiscard]] QByteArray encodeResponse(const Response& response);
[[nodiscard]] std::optional<Response> decodeResponse(const QByteArray& line);

} // namespace qttutorial::networking
