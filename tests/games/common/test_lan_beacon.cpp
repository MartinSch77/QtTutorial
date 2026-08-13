// SPDX-License-Identifier: MIT
#include "LanBeacon.h"

#include <QTest>

using namespace qttutorial::games::common;

class TestLanBeacon : public QObject {
    Q_OBJECT
private slots:
    void roundTripsAllFields()
    {
        LanBeacon beacon;
        beacon.gameId = QStringLiteral("schafkopf");
        beacon.hostName = QStringLiteral("Martin's table");
        beacon.tcpPort = 51234;
        beacon.seatsTaken = 2;
        beacon.seatsTotal = 4;

        const auto decoded = LanBeacon::decode(beacon.encode());
        QVERIFY(decoded.has_value());
        QCOMPARE(decoded->gameId, beacon.gameId);
        QCOMPARE(decoded->hostName, beacon.hostName);
        QCOMPARE(decoded->tcpPort, beacon.tcpPort);
        QCOMPARE(decoded->seatsTaken, beacon.seatsTaken);
        QCOMPARE(decoded->seatsTotal, beacon.seatsTotal);
    }

    void rejectsMissingRequiredField()
    {
        const auto decoded = LanBeacon::decode(R"({"hostName": "x"})");
        QVERIFY(!decoded.has_value());
        QCOMPARE(decoded.error(), LanBeacon::DecodeError::MissingField);
    }

    void rejectsGarbage()
    {
        const auto decoded = LanBeacon::decode("{{{not json");
        QVERIFY(!decoded.has_value());
        QCOMPARE(decoded.error(), LanBeacon::DecodeError::InvalidJson);
    }
};

QTEST_APPLESS_MAIN(TestLanBeacon)
#include "test_lan_beacon.moc"
