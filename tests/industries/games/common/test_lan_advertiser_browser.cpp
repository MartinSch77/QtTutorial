// SPDX-License-Identifier: MIT
#include "LanAdvertiser.h"
#include "LanBrowser.h"

#include <QSignalSpy>
#include <QTest>

using namespace qttutorial::games::common;

class TestLanAdvertiserBrowser : public QObject {
    Q_OBJECT
private slots:
    void browserDiscoversAdvertisedGame()
    {
        LanBrowser browser;
        if (!browser.start()) {
            QSKIP("Could not bind the LAN discovery port in this environment (likely sandboxed).");
        }

        LanAdvertiser advertiser;
        LanBeacon beacon;
        beacon.gameId = QStringLiteral("kicker");
        beacon.hostName = QStringLiteral("Test host");
        beacon.tcpPort = 12345;
        beacon.seatsTotal = 4;

        QSignalSpy gamesChangedSpy(&browser, &LanBrowser::gamesChanged);
        advertiser.start(beacon);

        QTRY_VERIFY(gamesChangedSpy.count() > 0);
        const auto games = browser.discoveredGames(QStringLiteral("kicker"));
        QCOMPARE(games.size(), 1);
        QCOMPARE(games.first().beacon.hostName, beacon.hostName);
        QCOMPARE(games.first().beacon.tcpPort, beacon.tcpPort);
    }

    void filtersByGameId()
    {
        LanBrowser browser;
        if (!browser.start()) {
            QSKIP("Could not bind the LAN discovery port in this environment (likely sandboxed).");
        }

        LanAdvertiser advertiser;
        LanBeacon beacon;
        beacon.gameId = QStringLiteral("schafkopf");
        beacon.tcpPort = 22222;

        QSignalSpy gamesChangedSpy(&browser, &LanBrowser::gamesChanged);
        advertiser.start(beacon);
        QTRY_VERIFY(gamesChangedSpy.count() > 0);

        QCOMPARE(browser.discoveredGames(QStringLiteral("kicker")).size(), 0);
        QCOMPARE(browser.discoveredGames(QStringLiteral("schafkopf")).size(), 1);
    }
};

QTEST_MAIN(TestLanAdvertiserBrowser)
#include "test_lan_advertiser_browser.moc"
