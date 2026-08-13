// SPDX-License-Identifier: MIT
#pragma once

#include "KickerMatch.h"
#include "LanBrowser.h"

#include <QObject>
#include <QQmlEngine>
#include <QVariantList>

namespace qttutorial::games::kicker {

// The QML-facing glue for one kicker match: owns a KickerMatch (host or
// client) and a LanBrowser for the lobby screen, translates raw mouse
// input into rod commands (see the README's "Controls" section for the
// exact mapping), and republishes MatchState as flat Q_PROPERTYs that
// qml/Table3D.qml and qml/Hud.qml bind against directly.
//
// This class is intentionally UI-layer only (not part of kicker_lib's
// pure/tested core): it is exercised by running the app, not by QTest.
class KickerController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int scoreA READ scoreA NOTIFY matchStateChanged)
    Q_PROPERTY(int scoreB READ scoreB NOTIFY matchStateChanged)
    Q_PROPERTY(double ballX READ ballX NOTIFY matchStateChanged)
    Q_PROPERTY(double ballZ READ ballZ NOTIFY matchStateChanged)
    Q_PROPERTY(double rod0Slide READ rod0Slide NOTIFY matchStateChanged)
    Q_PROPERTY(double rod0Rotation READ rod0Rotation NOTIFY matchStateChanged)
    Q_PROPERTY(double rod1Slide READ rod1Slide NOTIFY matchStateChanged)
    Q_PROPERTY(double rod1Rotation READ rod1Rotation NOTIFY matchStateChanged)
    Q_PROPERTY(double rod2Slide READ rod2Slide NOTIFY matchStateChanged)
    Q_PROPERTY(double rod2Rotation READ rod2Rotation NOTIFY matchStateChanged)
    Q_PROPERTY(double rod3Slide READ rod3Slide NOTIFY matchStateChanged)
    Q_PROPERTY(double rod3Rotation READ rod3Rotation NOTIFY matchStateChanged)
    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)
    Q_PROPERTY(int localSeat READ localSeat NOTIFY connectionStatusChanged)
    Q_PROPERTY(QVariantList lobbyGames READ lobbyGames NOTIFY lobbyGamesChanged)
public:
    explicit KickerController(QObject* parent = nullptr);

    [[nodiscard]] int scoreA() const { return m_match->state().score[0]; }
    [[nodiscard]] int scoreB() const { return m_match->state().score[1]; }
    [[nodiscard]] double ballX() const { return m_match->state().ball.x; }
    [[nodiscard]] double ballZ() const { return m_match->state().ball.z; }
    [[nodiscard]] double rod0Slide() const { return m_match->state().rods[0].slide; }
    [[nodiscard]] double rod0Rotation() const { return m_match->state().rods[0].rotation; }
    [[nodiscard]] double rod1Slide() const { return m_match->state().rods[1].slide; }
    [[nodiscard]] double rod1Rotation() const { return m_match->state().rods[1].rotation; }
    [[nodiscard]] double rod2Slide() const { return m_match->state().rods[2].slide; }
    [[nodiscard]] double rod2Rotation() const { return m_match->state().rods[2].rotation; }
    [[nodiscard]] double rod3Slide() const { return m_match->state().rods[3].slide; }
    [[nodiscard]] double rod3Rotation() const { return m_match->state().rods[3].rotation; }
    [[nodiscard]] QString connectionStatus() const { return m_connectionStatus; }
    [[nodiscard]] int localSeat() const { return m_match->localSeat(); }
    [[nodiscard]] QVariantList lobbyGames() const;

    Q_INVOKABLE bool hostMatch(int humanSeatCount);
    Q_INVOKABLE void joinManual(const QString& hostAddress, int port);
    Q_INVOKABLE void joinDiscovered(int index);
    Q_INVOKABLE void leaveMatch();

    // normalizedSlide in [-1, 1] is the mouse's position across the
    // table's width; spinRate is the mouse's horizontal speed in
    // normalized-widths-per-second. See README "Controls".
    Q_INVOKABLE void mouseInput(double normalizedSlide, double spinRate);

signals:
    void matchStateChanged();
    void connectionStatusChanged();
    void lobbyGamesChanged();

private:
    void setConnectionStatus(QString status);

    KickerMatch* m_match;
    common::LanBrowser* m_browser;
    RodState m_heldRod2p{}; // last commanded state of the currently-inactive rod, 2-player mode only
    QString m_connectionStatus = QStringLiteral("disconnected");
};

} // namespace qttutorial::games::kicker
