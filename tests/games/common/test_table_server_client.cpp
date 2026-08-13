// SPDX-License-Identifier: MIT
#include "TableClient.h"
#include "TableServer.h"

#include <QSignalSpy>
#include <QTest>

using namespace qttutorial::games::common;

class TestTableServerClient : public QObject {
    Q_OBJECT
private slots:
    void assignsSeatsInConnectionOrderAndRelaysMessages()
    {
        TableServer server(4);
        QVERIFY(server.listen());

        QSignalSpy seatConnectedSpy(&server, &TableServer::seatConnected);

        TableClient clientA;
        TableClient clientB;
        clientA.connectToHost(QStringLiteral("127.0.0.1"), server.serverPort());
        clientB.connectToHost(QStringLiteral("127.0.0.1"), server.serverPort());

        QTRY_COMPARE(seatConnectedSpy.count(), 2);
        QCOMPARE(server.seatCount(), 2);

        QSignalSpy serverReceivedSpy(&server, &TableServer::messageReceived);
        TableMessage fromA;
        fromA.type = QStringLiteral("ping");
        fromA.payload = QJsonObject{{QStringLiteral("n"), 1}};
        clientA.send(fromA);

        QTRY_COMPARE(serverReceivedSpy.count(), 1);
        const int seatA = serverReceivedSpy.at(0).at(0).toInt();
        const auto receivedMessage = serverReceivedSpy.at(0).at(1).value<TableMessage>();
        QCOMPARE(receivedMessage.type, QStringLiteral("ping"));
        QCOMPARE(receivedMessage.payload.value(QStringLiteral("n")).toInt(), 1);

        QSignalSpy clientBReceivedSpy(&clientB, &TableClient::messageReceived);
        TableMessage broadcastMessage;
        broadcastMessage.type = QStringLiteral("dealt");
        server.broadcast(broadcastMessage);
        QTRY_COMPARE(clientBReceivedSpy.count(), 1);

        QSignalSpy seatDisconnectedSpy(&server, &TableServer::seatDisconnected);
        clientA.disconnectFromHost();
        QTRY_COMPARE(seatDisconnectedSpy.count(), 1);
        QCOMPARE(seatDisconnectedSpy.at(0).at(0).toInt(), seatA);
    }

    void rejectsConnectionsBeyondMaxSeats()
    {
        TableServer server(1);
        QVERIFY(server.listen());

        TableClient clientA;
        TableClient clientB;
        QSignalSpy clientAConnected(&clientA, &TableClient::connected);
        QSignalSpy clientBDisconnected(&clientB, &TableClient::disconnected);

        clientA.connectToHost(QStringLiteral("127.0.0.1"), server.serverPort());
        QTRY_COMPARE(clientAConnected.count(), 1);

        clientB.connectToHost(QStringLiteral("127.0.0.1"), server.serverPort());
        QTRY_COMPARE(clientBDisconnected.count(), 1);
    }
};

QTEST_MAIN(TestTableServerClient)
#include "test_table_server_client.moc"
