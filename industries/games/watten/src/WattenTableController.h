// SPDX-License-Identifier: MIT
#pragma once

#include "LanAdvertiser.h"
#include "LanBrowser.h"
#include "TableClient.h"
#include "TableServer.h"
#include "WattenBot.h"
#include "WattenGame.h"

#include <QObject>
#include <QQmlEngine>
#include <QVariantList>

#include <array>
#include <memory>

namespace qttutorial::watten {

// The one class in this game that is allowed to know about both WattenGame
// (rules) and games_common_lib (LAN transport): it translates WattenGame's
// signals into the TableMessage protocol documented in README.md when
// hosting, translates incoming TableMessages back into WattenGame calls,
// and exposes the resulting state to QML as plain properties so the UI
// never has to know the protocol or the rules engine exists.
//
// Host-authoritative model: only the hosting process ever constructs a
// WattenGame. Joining processes are thin -- they show whatever the host
// broadcasts and send "play_card" requests, the host is the only one who
// validates and advances state. Any of the 4 seats not filled by a human
// (host or joined client) by the time the host starts the table is played
// by a local WattenBot the host drives directly, entirely outside the
// network protocol.
class WattenTableController : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(bool isHost READ isHost NOTIFY roleChanged)
    Q_PROPERTY(int mySeat READ mySeat NOTIFY roleChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(int dealerSeat READ dealerSeat NOTIFY dealerSeatChanged)
    Q_PROPERTY(int turnSeat READ turnSeat NOTIFY turnSeatChanged)
    Q_PROPERTY(QString trumpSuit READ trumpSuit NOTIFY trumpSuitChanged)
    Q_PROPERTY(int scoreTeamA READ scoreTeamA NOTIFY scoresChanged)
    Q_PROPERTY(int scoreTeamB READ scoreTeamB NOTIFY scoresChanged)
    Q_PROPERTY(QString bummerlWinner READ bummerlWinner NOTIFY bummerlWinnerChanged)
    Q_PROPERTY(QVariantList myHand READ myHand NOTIFY myHandChanged)
    Q_PROPERTY(QVariantList trick READ trick NOTIFY trickChanged)
    Q_PROPERTY(QVariantList lanGames READ lanGames NOTIFY lanGamesChanged)

public:
    explicit WattenTableController(QObject* parent = nullptr);
    ~WattenTableController() override;

    [[nodiscard]] bool isHost() const { return m_isHost; }
    [[nodiscard]] int mySeat() const { return m_mySeat; }
    [[nodiscard]] QString statusMessage() const { return m_statusMessage; }
    [[nodiscard]] int dealerSeat() const { return m_dealerSeat; }
    [[nodiscard]] int turnSeat() const { return m_turnSeat; }
    [[nodiscard]] QString trumpSuit() const { return m_trumpSuit; }
    [[nodiscard]] int scoreTeamA() const { return m_scoreTeamA; }
    [[nodiscard]] int scoreTeamB() const { return m_scoreTeamB; }
    [[nodiscard]] QString bummerlWinner() const { return m_bummerlWinner; }
    [[nodiscard]] QVariantList myHand() const { return m_myHand; }
    [[nodiscard]] QVariantList trick() const { return m_trick; }
    [[nodiscard]] QVariantList lanGames() const { return m_lanGames; }

    Q_INVOKABLE void hostGame(const QString& hostName);
    Q_INVOKABLE void startTable();
    Q_INVOKABLE void browseLan();
    Q_INVOKABLE void joinDiscovered(int index);
    Q_INVOKABLE void joinManual(const QString& address, int port);
    Q_INVOKABLE void playCard(const QString& suit, const QString& rank);

private:
    void setupHostGameSignals();
    void broadcastHandDealt();
    void applyHandStarted(int dealerSeat);
    void applyTrumpDecided(const QString& suit);
    void applyCardPlayed(int seat, const QString& suit, const QString& rank);
    void applyTurnChanged(int seat);
    void applyHandWon(const QString& team, int scoreA, int scoreB);
    void applyBummerlWon(const QString& team);
    void refreshMyHandFromGame();
    void refreshTrickFromGame();
    void driveBotsIfNeeded();
    void onServerMessage(int seat, const games::common::TableMessage& message);
    void onClientMessage(const games::common::TableMessage& message);

    bool m_isHost = false;
    int m_mySeat = -1;
    QString m_statusMessage;
    int m_dealerSeat = -1;
    int m_turnSeat = -1;
    QString m_trumpSuit;
    int m_scoreTeamA = 0;
    int m_scoreTeamB = 0;
    QString m_bummerlWinner;
    QVariantList m_myHand;
    QVariantList m_trick;
    QVariantList m_lanGames;

    std::unique_ptr<WattenGame> m_game;
    std::array<std::unique_ptr<WattenBot>, WattenGame::kSeatCount> m_bots;
    std::array<bool, WattenGame::kSeatCount> m_seatIsBot{true, true, true, true};

    std::unique_ptr<games::common::TableServer> m_server;
    std::unique_ptr<games::common::TableClient> m_client;
    std::unique_ptr<games::common::LanAdvertiser> m_advertiser;
    std::unique_ptr<games::common::LanBrowser> m_browser;

signals:
    void roleChanged();
    void statusMessageChanged();
    void dealerSeatChanged();
    void turnSeatChanged();
    void trumpSuitChanged();
    void scoresChanged();
    void bummerlWinnerChanged();
    void myHandChanged();
    void trickChanged();
    void lanGamesChanged();
};

} // namespace qttutorial::watten
