// SPDX-License-Identifier: MIT
#include "TableMessage.h"

#include <QTest>

using namespace qttutorial::games::common;

class TestTableMessage : public QObject {
    Q_OBJECT
private slots:
    void roundTripsTypeSeatAndPayload()
    {
        TableMessage message;
        message.type = QStringLiteral("playCard");
        message.seat = 2;
        message.payload = QJsonObject{{QStringLiteral("cardId"), QStringLiteral("EO")}};

        const auto decoded = TableMessage::fromLine(message.toLine());
        QVERIFY(decoded.has_value());
        QCOMPARE(decoded->type, message.type);
        QCOMPARE(decoded->seat, message.seat);
        QCOMPARE(decoded->payload, message.payload);
    }

    void rejectsInvalidJson()
    {
        const auto decoded = TableMessage::fromLine("not json");
        QVERIFY(!decoded.has_value());
        QCOMPARE(decoded.error(), TableMessage::DecodeError::InvalidJson);
    }

    void rejectsMissingType()
    {
        const auto decoded = TableMessage::fromLine(R"({"seat": 1})");
        QVERIFY(!decoded.has_value());
        QCOMPARE(decoded.error(), TableMessage::DecodeError::MissingType);
    }

    void defaultsSeatToMinusOneWhenAbsent()
    {
        const auto decoded = TableMessage::fromLine(R"({"type": "hello"})");
        QVERIFY(decoded.has_value());
        QCOMPARE(decoded->seat, -1);
    }
};

QTEST_APPLESS_MAIN(TestTableMessage)
#include "test_table_message.moc"
