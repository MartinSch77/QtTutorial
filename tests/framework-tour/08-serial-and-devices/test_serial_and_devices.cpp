// SPDX-License-Identifier: MIT
#include "DeviceLink.h"
#include "FrameCodec.h"
#include "FrameParser.h"
#include "SimulatedSerialLink.h"

#include <QLocalSocket>
#include <QSignalSpy>
#include <QTest>

using namespace qttutorial::serial_and_devices;

class TestSerialAndDevices : public QObject {
    Q_OBJECT
private slots:
    void checksumIsDeterministicAndSensitiveToChanges()
    {
        const QByteArray a = "hello";
        const QByteArray b = "hellp"; // last byte differs by one bit pattern
        QCOMPARE(computeChecksum(a), computeChecksum(a));
        QVERIFY(computeChecksum(a) != computeChecksum(b));
        QCOMPARE(computeChecksum(QByteArray()), quint8(0));
    }

    void encodeFrameProducesExpectedLayout()
    {
        const QByteArray payload = "hi";
        const QByteArray frame = encodeFrame(payload);
        QCOMPARE(frame.size(), payload.size() + 4);
        QCOMPARE(static_cast<unsigned char>(frame.at(0)), kFrameStx);
        QCOMPARE(static_cast<unsigned char>(frame.at(1)), static_cast<unsigned char>(payload.size()));
        QCOMPARE(frame.mid(2, payload.size()), payload);
        QCOMPARE(static_cast<quint8>(frame.at(frame.size() - 2)), computeChecksum(payload));
        QCOMPARE(static_cast<unsigned char>(frame.at(frame.size() - 1)), kFrameEtx);
    }

    void encodeFrameRejectsOversizedPayload()
    {
        const QByteArray oversized(kMaxPayloadSize + 1, 'x');
        QVERIFY(encodeFrame(oversized).isEmpty());
    }

    void parserReassemblesFrameFedInSmallChunks()
    {
        FrameParser parser;
        QSignalSpy decodedSpy(&parser, &FrameParser::frameDecoded);
        QSignalSpy errorSpy(&parser, &FrameParser::frameError);

        const QByteArray frame = encodeFrame("chunked payload");
        for (const char byte : frame) {
            parser.feed(QByteArray(1, byte));
        }

        QCOMPARE(decodedSpy.count(), 1);
        QCOMPARE(errorSpy.count(), 0);
        QCOMPARE(decodedSpy.at(0).at(0).toByteArray(), QByteArray("chunked payload"));
    }

    void parserHandlesMultipleFramesInOneChunk()
    {
        FrameParser parser;
        QSignalSpy decodedSpy(&parser, &FrameParser::frameDecoded);

        const QByteArray combined = encodeFrame("one") + encodeFrame("two");
        parser.feed(combined);

        QCOMPARE(decodedSpy.count(), 2);
        QCOMPARE(decodedSpy.at(0).at(0).toByteArray(), QByteArray("one"));
        QCOMPARE(decodedSpy.at(1).at(0).toByteArray(), QByteArray("two"));
    }

    void parserFlagsChecksumMismatchWithoutCrashing()
    {
        FrameParser parser;
        QSignalSpy decodedSpy(&parser, &FrameParser::frameDecoded);
        QSignalSpy errorSpy(&parser, &FrameParser::frameError);

        QByteArray frame = encodeFrame("corrupt me");
        const qsizetype checksumIndex = frame.size() - 2;
        frame[checksumIndex] = static_cast<char>(~frame[checksumIndex]);

        parser.feed(frame);

        QCOMPARE(decodedSpy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
        QVERIFY(errorSpy.at(0).at(0).toString().contains("checksum", Qt::CaseInsensitive));
    }

    void parserWaitsForIncompleteFrame()
    {
        FrameParser parser;
        QSignalSpy decodedSpy(&parser, &FrameParser::frameDecoded);

        const QByteArray frame = encodeFrame("not yet complete");
        parser.feed(frame.left(frame.size() - 1));
        QCOMPARE(decodedSpy.count(), 0);

        parser.feed(frame.right(1));
        QCOMPARE(decodedSpy.count(), 1);
    }

    void simulatedLinkMovesBytesBetweenBothEnds()
    {
        SimulatedSerialLink link;
        QVERIFY(link.isConnected());

        DeviceLink deviceSide(link.deviceEnd(), 1000);
        QSignalSpy receivedSpy(&deviceSide, &DeviceLink::frameReceived);

        link.clientEnd()->write(encodeFrame("ping over simulated wire"));

        QTRY_COMPARE(receivedSpy.count(), 1);
        QCOMPARE(receivedSpy.at(0).at(0).toByteArray(), QByteArray("ping over simulated wire"));
    }

    void deviceLinkEchoRoundTripSucceeds()
    {
        SimulatedSerialLink link;
        QVERIFY(link.isConnected());

        DeviceLink controllerLink(link.clientEnd(), 1000);
        DeviceLink deviceSideLink(link.deviceEnd(), 1000);

        connect(&deviceSideLink, &DeviceLink::frameReceived, &deviceSideLink, [&deviceSideLink](const QByteArray& payload) {
            deviceSideLink.sendFrame(payload);
        });

        QSignalSpy controllerReceivedSpy(&controllerLink, &DeviceLink::frameReceived);
        QSignalSpy controllerTimedOutSpy(&controllerLink, &DeviceLink::timedOut);

        controllerLink.sendFrame("round trip payload");

        QTRY_COMPARE(controllerReceivedSpy.count(), 1);
        QCOMPARE(controllerTimedOutSpy.count(), 0);
        QCOMPARE(controllerReceivedSpy.at(0).at(0).toByteArray(), QByteArray("round trip payload"));
    }

    void deviceLinkTimesOutWithoutAResponse()
    {
        SimulatedSerialLink link;
        QVERIFY(link.isConnected());

        // Deliberately do not attach anything on the device end that would reply,
        // so the controller side must hit its own timeout.
        DeviceLink controllerLink(link.clientEnd(), 200);
        QSignalSpy timedOutSpy(&controllerLink, &DeviceLink::timedOut);
        QSignalSpy receivedSpy(&controllerLink, &DeviceLink::frameReceived);

        controllerLink.sendFrame("nobody is listening");

        QTRY_COMPARE(timedOutSpy.count(), 1);
        QCOMPARE(receivedSpy.count(), 0);
    }
};

QTEST_MAIN(TestSerialAndDevices)
#include "test_serial_and_devices.moc"
