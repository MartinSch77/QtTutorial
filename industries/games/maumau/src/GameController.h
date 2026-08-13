// SPDX-License-Identifier: MIT
#pragma once

#include "MauMauGame.h"

#include <LanAdvertiser.h>
#include <LanBrowser.h>
#include <TableClient.h>
#include <TableServer.h>

#include <QObject>
#include <QQmlEngine>
#include <QVariantList>

namespace qttutorial::maumau {

inline constexpr int kSeatCount = 4;

// The QML-facing glue: owns the LAN transport and, when hosting, the authoritative MauMauGame.
// The host's own seat is not special-cased: hostGame() also opens a TableClient connecting to
// its own TableServer over 127.0.0.1, so the host's UI drives its seat through exactly the same
// TableMessage send/receive path a remote peer uses -- MauMauGame never has to know whether a
// seat's messages came from the network or from the process hosting it. See README.md for why
// this makes the host reliably claim seat 0 (it connects to itself before advertising).
class GameController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool inGame READ inGame NOTIFY inGameChanged)
    Q_PROPERTY(bool isHost READ isHost NOTIFY inGameChanged)
    Q_PROPERTY(int mySeat READ mySeat NOTIFY mySeatChanged)
    Q_PROPERTY(int seatCount READ seatCount CONSTANT)
    Q_PROPERTY(int currentSeat READ currentSeat NOTIFY publicStateChanged)
    Q_PROPERTY(QString phase READ phase NOTIFY publicStateChanged)
    Q_PROPERTY(int winnerSeat READ winnerSeat NOTIFY publicStateChanged)
    Q_PROPERTY(QString topCardRank READ topCardRank NOTIFY publicStateChanged)
    Q_PROPERTY(QString topCardSuit READ topCardSuit NOTIFY publicStateChanged)
    Q_PROPERTY(QString wishedSuit READ wishedSuit NOTIFY publicStateChanged)
    Q_PROPERTY(int drawPileCount READ drawPileCount NOTIFY publicStateChanged)
    Q_PROPERTY(QVariantList handCounts READ handCounts NOTIFY publicStateChanged)
    Q_PROPERTY(QVariantList myHand READ myHand NOTIFY myHandChanged)
    Q_PROPERTY(QString lastNotice READ lastNotice NOTIFY lastNoticeChanged)
    Q_PROPERTY(QString connectionError READ connectionError NOTIFY connectionErrorChanged)
    Q_PROPERTY(QVariantList discoveredGames READ discoveredGames NOTIFY discoveredGamesChanged)
public:
    explicit GameController(QObject* parent = nullptr);

    [[nodiscard]] bool inGame() const { return m_inGame; }
    [[nodiscard]] bool isHost() const { return m_isHost; }
    [[nodiscard]] int mySeat() const { return m_mySeat; }
    [[nodiscard]] int seatCount() const { return kSeatCount; }
    [[nodiscard]] int currentSeat() const { return m_currentSeat; }
    [[nodiscard]] QString phase() const { return m_phase; }
    [[nodiscard]] int winnerSeat() const { return m_winnerSeat; }
    [[nodiscard]] QString topCardRank() const { return m_topCardRank; }
    [[nodiscard]] QString topCardSuit() const { return m_topCardSuit; }
    [[nodiscard]] QString wishedSuit() const { return m_wishedSuit; }
    [[nodiscard]] int drawPileCount() const { return m_drawPileCount; }
    [[nodiscard]] QVariantList handCounts() const { return m_handCounts; }
    [[nodiscard]] QVariantList myHand() const { return m_myHand; }
    [[nodiscard]] QString lastNotice() const { return m_lastNotice; }
    [[nodiscard]] QString connectionError() const { return m_connectionError; }
    [[nodiscard]] QVariantList discoveredGames() const { return m_discoveredGames; }

    Q_INVOKABLE void hostGame(const QString& hostName);
    Q_INVOKABLE void startRound();
    Q_INVOKABLE void refreshDiscovery();
    Q_INVOKABLE void joinDiscovered(int index);
    Q_INVOKABLE void joinManual(const QString& address, int port);
    Q_INVOKABLE void playCard(const QString& rank, const QString& suit, const QString& wish);
    Q_INVOKABLE void drawCard();
    Q_INVOKABLE [[nodiscard]] bool isCardLegal(const QString& rank, const QString& suit) const;

signals:
    void inGameChanged();
    void mySeatChanged();
    void publicStateChanged();
    void myHandChanged();
    void lastNoticeChanged();
    void connectionErrorChanged();
    void discoveredGamesChanged();

private:
    void applyPublicState(const games::common::TableMessage& message);
    void applyHand(const games::common::TableMessage& message);
    void applyNotice(const games::common::TableMessage& message);
    void applyError(const games::common::TableMessage& message);
    void updateDiscoveredGames();

    bool m_inGame = false;
    bool m_isHost = false;
    int m_mySeat = -1;
    int m_currentSeat = 0;
    QString m_phase = QStringLiteral("lobby");
    int m_winnerSeat = -1;
    QString m_topCardRank;
    QString m_topCardSuit;
    QString m_wishedSuit;
    int m_drawPileCount = 0;
    QVariantList m_handCounts;
    QVariantList m_myHand;
    QString m_lastNotice;
    QString m_connectionError;
    QVariantList m_discoveredGames;

    MauMauGame m_game;
    games::common::TableServer m_server{kSeatCount};
    games::common::TableClient m_client;
    games::common::LanAdvertiser m_advertiser;
    games::common::LanBrowser m_browser;
};

} // namespace qttutorial::maumau
