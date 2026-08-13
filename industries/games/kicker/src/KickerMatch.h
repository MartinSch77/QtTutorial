// SPDX-License-Identifier: MIT
#pragma once

#include "KickerProtocol.h"
#include "KickerSimulation.h"
#include "KickerTypes.h"

#include "LanAdvertiser.h"
#include "TableClient.h"
#include "TableMessage.h"
#include "TableServer.h"

#include <QObject>
#include <QTimer>

#include <memory>

namespace qttutorial::games::kicker {

inline constexpr int kSimulationTickMs = 20; // 50 Hz authoritative tick
inline constexpr int kHostLogicalSeat = 0;

// Runs one match: as host, ticks the authoritative KickerSimulation and
// broadcasts "state" snapshots; as client, sends "input" messages and
// mirrors whatever "state" snapshot the host last broadcast. See the
// kicker README's "Network protocol" section for the exact TableMessage
// shapes.
class KickerMatch : public QObject {
    Q_OBJECT
public:
    explicit KickerMatch(QObject* parent = nullptr);
    ~KickerMatch() override;

    [[nodiscard]] bool hostMatch(int humanSeatCount, quint16 port = 0);
    void joinMatch(const QString& hostAddress, quint16 port);
    void leaveMatch();

    // Called by the UI layer with the freshly mouse-derived target(s) for
    // the rod(s) the local player currently controls. As host this is
    // applied to the simulation directly; as client it is sent to the
    // host as an "input" message.
    void setLocalInputs(const QList<RodInputEntry>& entries);

    [[nodiscard]] bool isHost() const { return m_server != nullptr; }
    [[nodiscard]] int localSeat() const { return m_localSeat; }
    [[nodiscard]] int humanSeatCount() const { return m_humanSeatCount; }
    [[nodiscard]] quint16 hostedPort() const;
    [[nodiscard]] const MatchState& state() const { return m_state; }

signals:
    void stateChanged();
    void seatAssigned(int seat);
    void seatConnected(int seat);
    void seatDisconnected(int seat);
    void connectedToHost();
    void disconnectedFromHost();
    void connectionError(QString message);

private:
    void onSimulationTick();
    void onServerMessage(int transportSeat, const common::TableMessage& message);
    void onClientMessage(const common::TableMessage& message);
    void applyInputEntries(int logicalSeat, const QList<RodInputEntry>& entries);

    std::unique_ptr<common::TableServer> m_server;
    std::unique_ptr<common::TableClient> m_client;
    common::LanAdvertiser m_advertiser;
    QTimer m_tickTimer;

    int m_localSeat = -1;
    int m_humanSeatCount = 0;

    MatchState m_state;
    std::array<RodInput, 4> m_inputs{};
};

} // namespace qttutorial::games::kicker
