// SPDX-License-Identifier: MIT
#include "EchoServer.h"
#include "Protocol.h"

#include <QDateTime>
#include <QHostAddress>
#include <QSignalSpy>
#include <QTcpSocket>
#include <QTest>

using namespace qttutorial::networking;

class TestNetworkingProtocol : public QObject {
    Q_OBJECT
private slots:
    void roundTripsRequest()
    {
        Request request;
        request.id = 42;
        request.clientTimestampMs = 1234567890;

        const QByteArray encoded = encodeRequest(request);
        QVERIFY(encoded.endsWith('\n'));

        const std::optional<Request> decoded = decodeRequest(encoded);
        QVERIFY(decoded.has_value());
        QCOMPARE(decoded->id, request.id);
        QCOMPARE(decoded->clientTimestampMs, request.clientTimestampMs);
    }

    void roundTripsResponse()
    {
        Response response;
        response.id = 7;
        response.clientTimestampMs = 111;
        response.serverTimestampMs = 222;
        response.echo = QStringLiteral("echo of request 7");

        const QByteArray encoded = encodeResponse(response);
        const std::optional<Response> decoded = decodeResponse(encoded);
        QVERIFY(decoded.has_value());
        QCOMPARE(decoded->id, response.id);
        QCOMPARE(decoded->clientTimestampMs, response.clientTimestampMs);
        QCOMPARE(decoded->serverTimestampMs, response.serverTimestampMs);
        QCOMPARE(decoded->echo, response.echo);
    }

    void rejectsMalformedInput()
    {
        QVERIFY(!decodeRequest(QByteArrayLiteral("not json")).has_value());
        QVERIFY(!decodeResponse(QByteArrayLiteral("{\"id\":1}")).has_value());
    }

    void echoServerAnswersRealClient()
    {
        EchoServer server;
        QVERIFY(server.listen(QHostAddress::LocalHost, 0));

        QTcpSocket client;
        client.connectToHost(QHostAddress::LocalHost, server.serverPort());
        QVERIFY(client.waitForConnected(2000));

        Request request;
        request.id = 99;
        request.clientTimestampMs = QDateTime::currentMSecsSinceEpoch();
        client.write(encodeRequest(request));

        QSignalSpy readySpy(&client, &QTcpSocket::readyRead);
        QTRY_VERIFY(client.canReadLine() || readySpy.count() > 0);

        QByteArray line;
        QTRY_VERIFY(client.canReadLine());
        line = client.readLine();

        const std::optional<Response> response = decodeResponse(line);
        QVERIFY(response.has_value());
        QCOMPARE(response->id, request.id);
        QCOMPARE(response->clientTimestampMs, request.clientTimestampMs);
        QVERIFY(response->serverTimestampMs > 0);
        QVERIFY(response->echo.contains(QString::number(request.id)));
    }
};

QTEST_MAIN(TestNetworkingProtocol)
#include "test_networking_protocol.moc"
