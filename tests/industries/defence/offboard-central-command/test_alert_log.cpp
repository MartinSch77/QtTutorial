// SPDX-License-Identifier: MIT
#include "AlertLog.h"

#include <QTest>

using namespace qttutorial::defence;

class TestAlertLog : public QObject {
    Q_OBJECT
private slots:
    void ordersBySeverityCriticalFirst()
    {
        AlertLog log;
        const QDateTime now = QDateTime::currentDateTimeUtc();

        log.addAlert(Alert{QStringLiteral("A"), QStringLiteral("info msg"), AlertSeverity::Info, now});
        log.addAlert(Alert{QStringLiteral("B"), QStringLiteral("critical msg"), AlertSeverity::Critical, now});
        log.addAlert(Alert{QStringLiteral("C"), QStringLiteral("caution msg"), AlertSeverity::Caution, now});

        const auto sorted = log.alertsBySeverity();
        QCOMPARE(sorted.size(), std::size_t(3));
        QCOMPARE(sorted[0].severity, AlertSeverity::Critical);
        QCOMPARE(sorted[1].severity, AlertSeverity::Caution);
        QCOMPARE(sorted[2].severity, AlertSeverity::Info);
    }

    void ordersMostRecentFirstWithinSameSeverity()
    {
        AlertLog log;
        const QDateTime earlier = QDateTime::currentDateTimeUtc();
        const QDateTime later = earlier.addSecs(10);

        log.addAlert(Alert{QStringLiteral("A"), QStringLiteral("first"), AlertSeverity::Caution, earlier});
        log.addAlert(Alert{QStringLiteral("B"), QStringLiteral("second"), AlertSeverity::Caution, later});

        const auto sorted = log.alertsBySeverity();
        QCOMPARE(sorted.size(), std::size_t(2));
        QCOMPARE(sorted[0].assetId, QStringLiteral("B"));
        QCOMPARE(sorted[1].assetId, QStringLiteral("A"));
    }

    void emptyLogProducesEmptyResult()
    {
        AlertLog log;
        QVERIFY(log.alertsBySeverity().empty());
        QCOMPARE(log.size(), std::size_t(0));
    }
};

QTEST_MAIN(TestAlertLog)
#include "test_alert_log.moc"
