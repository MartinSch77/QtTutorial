// SPDX-License-Identifier: MIT
#include "KickerController.h"

#include "KickerSimulation.h"

namespace qttutorial::games::kicker {

using common::DiscoveredGame;

KickerController::KickerController(QObject* parent)
    : QObject(parent)
    , m_match(new KickerMatch(this))
    , m_browser(new common::LanBrowser(this))
{
    connect(m_match, &KickerMatch::stateChanged, this, &KickerController::matchStateChanged);
    connect(m_match, &KickerMatch::seatAssigned, this,
            [this](int) { setConnectionStatus(m_match->isHost() ? QStringLiteral("hosting")
                                                                  : QStringLiteral("connected")); });
    connect(m_match, &KickerMatch::connectedToHost, this,
            [this] { setConnectionStatus(QStringLiteral("connected")); });
    connect(m_match, &KickerMatch::disconnectedFromHost, this,
            [this] { setConnectionStatus(QStringLiteral("disconnected")); });
    connect(m_match, &KickerMatch::connectionError, this,
            [this](const QString& message) { setConnectionStatus(QStringLiteral("error: ") + message); });
    connect(m_match, &KickerMatch::seatConnected, this, &KickerController::matchStateChanged);
    connect(m_match, &KickerMatch::seatDisconnected, this, &KickerController::matchStateChanged);
    connect(m_browser, &common::LanBrowser::gamesChanged, this, &KickerController::lobbyGamesChanged);
    if (!m_browser->start()) {
        setConnectionStatus(QStringLiteral("error: LAN discovery unavailable"));
    }
}

QVariantList KickerController::lobbyGames() const
{
    QVariantList list;
    for (const DiscoveredGame& game : m_browser->discoveredGames(QStringLiteral("kicker"))) {
        list.append(QVariantMap{
            {QStringLiteral("hostAddress"), game.hostAddress},
            {QStringLiteral("hostName"), game.beacon.hostName},
            {QStringLiteral("tcpPort"), game.beacon.tcpPort},
            {QStringLiteral("seatsTaken"), game.beacon.seatsTaken},
            {QStringLiteral("seatsTotal"), game.beacon.seatsTotal},
        });
    }
    return list;
}

bool KickerController::hostMatch(int humanSeatCount)
{
    setConnectionStatus(QStringLiteral("hosting"));
    return m_match->hostMatch(humanSeatCount);
}

void KickerController::joinManual(const QString& hostAddress, int port)
{
    setConnectionStatus(QStringLiteral("connecting"));
    m_match->joinMatch(hostAddress, static_cast<quint16>(port));
}

void KickerController::joinDiscovered(int index)
{
    const auto games = m_browser->discoveredGames(QStringLiteral("kicker"));
    if (index < 0 || index >= games.size()) {
        return;
    }
    joinManual(games[index].hostAddress, games[index].beacon.tcpPort);
}

void KickerController::leaveMatch()
{
    m_match->leaveMatch();
    setConnectionStatus(QStringLiteral("disconnected"));
}

void KickerController::mouseInput(double normalizedSlide, double spinRate)
{
    const int seat = m_match->localSeat();
    if (seat < 0) {
        return;
    }

    const auto rods = KickerSimulation::rodsForSeat(seat, m_match->humanSeatCount());
    QList<RodInputEntry> entries;

    if (rods[1] < 0) {
        RodInputEntry entry;
        entry.rod = rods[0];
        entry.input.slideTarget = normalizedSlide * kRodSlideRange;
        entry.input.spin = spinRate * kMaxAngularSpeed;
        entries.append(entry);
    } else {
        const int active = KickerSimulation::selectActiveRod(m_match->state(), rods[0], rods[1]);
        const int idle = active == rods[0] ? rods[1] : rods[0];

        RodInputEntry activeEntry;
        activeEntry.rod = active;
        activeEntry.input.slideTarget = normalizedSlide * kRodSlideRange;
        activeEntry.input.spin = spinRate * kMaxAngularSpeed;
        entries.append(activeEntry);

        RodInputEntry idleEntry;
        idleEntry.rod = idle;
        idleEntry.input.slideTarget = m_match->state().rods[static_cast<std::size_t>(idle)].slide;
        idleEntry.input.spin = 0.0;
        entries.append(idleEntry);
    }

    m_match->setLocalInputs(entries);
}

void KickerController::setConnectionStatus(QString status)
{
    if (m_connectionStatus == status) {
        return;
    }
    m_connectionStatus = std::move(status);
    emit connectionStatusChanged();
}

} // namespace qttutorial::games::kicker
