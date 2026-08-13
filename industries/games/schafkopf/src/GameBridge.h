// SPDX-License-Identifier: MIT
#pragma once

#include "Bot.h"
#include "SchafkopfClientState.h"
#include "SchafkopfGame.h"

#include <LanAdvertiser.h>
#include <LanBrowser.h>
#include <TableClient.h>
#include <TableServer.h>

#include <QHash>
#include <QObject>
#include <QQmlEngine>
#include <QSet>
#include <QVariantList>

namespace qttutorial::games::schafkopf {

// The QML-facing app glue: owns the LAN transport (TableServer + LanAdvertiser
// when hosting, TableClient + LanBrowser when joining), the authoritative
// SchafkopfGame (hosting only), and a SchafkopfClientState per locally-driven
// seat (the UI's own seat, plus every seat currently played by a Bot).
// Deliberately not part of schafkopf_lib: this class is the one place in the
// game that is allowed to know about sockets.
class GameBridge : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString stage READ stage NOTIFY stageChanged)
    Q_PROPERTY(QString handPhase READ handPhase NOTIFY stateChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(bool isHost READ isHost NOTIFY stageChanged)
    Q_PROPERTY(int mySeat READ mySeat NOTIFY mySeatChanged)
    Q_PROPERTY(int hostPort READ hostPort NOTIFY stageChanged)
    Q_PROPERTY(int dealerSeat READ dealerSeat NOTIFY stateChanged)
    Q_PROPERTY(int biddingSeat READ biddingSeat NOTIFY stateChanged)
    Q_PROPERTY(int turnSeat READ turnSeat NOTIFY stateChanged)
    Q_PROPERTY(int callerSeat READ callerSeat NOTIFY stateChanged)
    Q_PROPERTY(QString calledSuitId READ calledSuitId NOTIFY stateChanged)
    Q_PROPERTY(bool isMyTurnToBid READ isMyTurnToBid NOTIFY stateChanged)
    Q_PROPERTY(bool isMyTurnToPlay READ isMyTurnToPlay NOTIFY stateChanged)
    Q_PROPERTY(QVariantList myHand READ myHand NOTIFY stateChanged)
    Q_PROPERTY(QVariantList legalCallSuits READ legalCallSuits NOTIFY stateChanged)
    Q_PROPERTY(QVariantList trick READ trick NOTIFY stateChanged)
    Q_PROPERTY(QVariantMap lastResult READ lastResult NOTIFY stateChanged)
    Q_PROPERTY(QVariantList discoveredGames READ discoveredGames NOTIFY discoveredGamesChanged)
public:
    explicit GameBridge(QObject* parent = nullptr);

    [[nodiscard]] QString stage() const { return m_stage; }
    [[nodiscard]] QString handPhase() const;
    [[nodiscard]] QString statusText() const { return m_statusText; }
    [[nodiscard]] bool isHost() const { return m_isHost; }
    [[nodiscard]] int mySeat() const { return m_mySeat; }
    [[nodiscard]] int hostPort() const;
    [[nodiscard]] int dealerSeat() const;
    [[nodiscard]] int biddingSeat() const;
    [[nodiscard]] int turnSeat() const;
    [[nodiscard]] int callerSeat() const;
    [[nodiscard]] QString calledSuitId() const;
    [[nodiscard]] bool isMyTurnToBid() const;
    [[nodiscard]] bool isMyTurnToPlay() const;
    [[nodiscard]] QVariantList myHand() const;
    [[nodiscard]] QVariantList legalCallSuits() const;
    [[nodiscard]] QVariantList trick() const;
    [[nodiscard]] QVariantMap lastResult() const;
    [[nodiscard]] QVariantList discoveredGames() const;

    Q_INVOKABLE void hostGame(const QString& playerName);
    Q_INVOKABLE void startDiscovery();
    Q_INVOKABLE void joinGame(const QString& hostAddress, int port, const QString& playerName);
    Q_INVOKABLE void joinDiscovered(int index, const QString& playerName);
    Q_INVOKABLE void startPlaying();
    Q_INVOKABLE void nextHand();
    Q_INVOKABLE void callSuit(const QString& suitId);
    Q_INVOKABLE void pass();
    Q_INVOKABLE void playCard(const QString& cardId);

signals:
    void stageChanged();
    void statusTextChanged();
    void mySeatChanged();
    void stateChanged();
    void discoveredGamesChanged();

private:
    [[nodiscard]] SchafkopfClientState* myState() const;
    [[nodiscard]] Bot* botFor(int seat);
    void deliverLocal(int seat, const qttutorial::games::common::TableMessage& message);
    void maybeActBot(int seat);
    void wireGame();
    void setStage(const QString& stage);
    void setStatusText(const QString& text);

    bool m_isHost = false;
    int m_mySeat = -1;
    QString m_stage = QStringLiteral("disconnected");
    QString m_statusText;

    qttutorial::games::common::TableServer* m_server = nullptr;
    qttutorial::games::common::TableClient* m_client = nullptr;
    qttutorial::games::common::LanAdvertiser* m_advertiser = nullptr;
    qttutorial::games::common::LanBrowser* m_browser = nullptr;
    SchafkopfGame* m_game = nullptr;

    QHash<int, SchafkopfClientState*> m_states;
    QHash<int, Bot*> m_bots;
    QSet<int> m_connectedServerSeats; // logical seats (1..3) with a live TableServer connection
};

} // namespace qttutorial::games::schafkopf
