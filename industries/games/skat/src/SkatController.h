// SPDX-License-Identifier: MIT
#pragma once

#include "SkatGame.h"

#include <LanAdvertiser.h>
#include <LanBrowser.h>
#include <TableClient.h>
#include <TableServer.h>

#include <QObject>
#include <QQmlEngine>
#include <QVariantList>
#include <QVariantMap>

#include <memory>

namespace qttutorial::games::skat {

// The QML-facing glue object: owns either a TableServer+SkatGame (hosting)
// or a TableClient (joining), and turns TableMessages into plain
// Q_PROPERTYs the UI can bind against. This class is deliberately the only
// place in industries/games/skat that talks to games_common_lib's network classes --
// SkatGame itself only ever sees TableMessage values, never a socket (see
// SkatGame.h). The host's own UI is driven through exactly the same
// handleIncomingMessage() path as a remote client's, since the host is
// simply "seat 0, plus the authority": SkatGame::send is looped back
// locally instead of round-tripping through a real socket.
class SkatController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString phase READ phase NOTIFY phaseChanged)
    Q_PROPERTY(int localSeat READ localSeat NOTIFY localSeatChanged)
    Q_PROPERTY(bool isHost READ isHost NOTIFY connectionChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectionChanged)
    Q_PROPERTY(QVariantMap tableInfo READ tableInfo NOTIFY tableChanged)
    Q_PROPERTY(QStringList myHand READ myHand NOTIFY handChanged)
    Q_PROPERTY(QVariantList currentTrick READ currentTrick NOTIFY trickChanged)
    Q_PROPERTY(QVariantMap biddingInfo READ biddingInfo NOTIFY biddingChanged)
    Q_PROPERTY(QVariantMap declarationInfo READ declarationInfo NOTIFY declarationChanged)
    Q_PROPERTY(QStringList skatForPickup READ skatForPickup NOTIFY skatChanged)
    Q_PROPERTY(QVariantMap lastTrickInfo READ lastTrickInfo NOTIFY lastTrickChanged)
    Q_PROPERTY(QVariantMap resultInfo READ resultInfo NOTIFY resultChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QVariantList discoveredGames READ discoveredGames NOTIFY discoveredGamesChanged)
public:
    explicit SkatController(QObject* parent = nullptr);

    [[nodiscard]] QString phase() const { return m_phase; }
    [[nodiscard]] int localSeat() const { return m_localSeat; }
    [[nodiscard]] bool isHost() const { return m_isHost; }
    [[nodiscard]] bool connected() const { return m_connected; }
    [[nodiscard]] QVariantMap tableInfo() const { return m_tableInfo; }
    [[nodiscard]] QStringList myHand() const { return m_myHand; }
    [[nodiscard]] QVariantList currentTrick() const { return m_currentTrick; }
    [[nodiscard]] QVariantMap biddingInfo() const { return m_biddingInfo; }
    [[nodiscard]] QVariantMap declarationInfo() const { return m_declarationInfo; }
    [[nodiscard]] QStringList skatForPickup() const { return m_skatForPickup; }
    [[nodiscard]] QVariantMap lastTrickInfo() const { return m_lastTrickInfo; }
    [[nodiscard]] QVariantMap resultInfo() const { return m_resultInfo; }
    [[nodiscard]] QString statusMessage() const { return m_statusMessage; }
    [[nodiscard]] QVariantList discoveredGames() const;

    Q_INVOKABLE void hostGame(const QString& hostName);
    Q_INVOKABLE void refreshDiscovery();
    Q_INVOKABLE void joinDiscoveredGame(int index, const QString& displayName);
    Q_INVOKABLE void joinManual(const QString& host, int port, const QString& displayName);
    Q_INVOKABLE void startNextHand();
    Q_INVOKABLE void submitBid(int amount);
    Q_INVOKABLE void submitPass();
    Q_INVOKABLE void submitDiscard(const QStringList& cards);
    Q_INVOKABLE void submitAnnouncement(const QString& gameType, const QString& trumpSuit);
    Q_INVOKABLE void playCard(const QString& cardCode);

signals:
    void phaseChanged();
    void localSeatChanged();
    void connectionChanged();
    void tableChanged();
    void handChanged();
    void trickChanged();
    void biddingChanged();
    void declarationChanged();
    void skatChanged();
    void lastTrickChanged();
    void resultChanged();
    void statusMessageChanged();
    void discoveredGamesChanged();

private:
    void handleIncomingMessage(const common::TableMessage& message);
    [[nodiscard]] bool isForMe(const common::TableMessage& message) const;
    void setPhase(const QString& phase);

    // One handler per TableMessage type, each dispatched to from
    // handleIncomingMessage(): keeps that dispatcher itself at a low,
    // easily-reviewed cyclomatic complexity instead of one long branch body
    // per message type.
    void handleSeatAssigned(const QJsonObject& payload);
    void handleLobby(const QJsonObject& payload);
    void handleHand(const common::TableMessage& message);
    void handleBiddingTurn(const QJsonObject& payload);
    void handleBiddingFinished(const QJsonObject& payload);
    void handleSkat(const common::TableMessage& message);
    void handleDiscardAccepted();
    void handleAnnounced(const QJsonObject& payload);
    void handleCardPlayed(const QJsonObject& payload);
    void handleTrickTurn(const QJsonObject& payload);
    void handleTrickFinished(const QJsonObject& payload);
    void handleHandFinished(const QJsonObject& payload);
    void handleActionRejected(const common::TableMessage& message);
    void sendAction(const QString& type, const QJsonObject& payload);

    bool m_isHost = false;
    bool m_connected = false;
    int m_localSeat = -1;
    QString m_phase = QStringLiteral("lobby");
    QString m_displayName;

    QVariantMap m_tableInfo;
    QStringList m_myHand;
    QVariantList m_currentTrick;
    QVariantMap m_biddingInfo;
    QVariantMap m_declarationInfo;
    QStringList m_skatForPickup;
    QVariantMap m_lastTrickInfo;
    QVariantMap m_resultInfo;
    QString m_statusMessage;

    std::unique_ptr<common::TableServer> m_server;
    std::unique_ptr<common::TableClient> m_client;
    std::unique_ptr<SkatGame> m_game;
    common::LanAdvertiser m_advertiser;
    common::LanBrowser m_browser;
};

} // namespace qttutorial::games::skat
