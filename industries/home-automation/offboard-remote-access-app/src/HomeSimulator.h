// SPDX-License-Identifier: MIT
#pragma once

#include "Protocol.h"

namespace qttutorial::homeautomation::remote {

// Stands in for "the house": the authoritative simulated device state that
// HomeLinkServer serves to remote clients. No Qt network/UI dependency, so
// the toggling logic is unit testable independent of the transport.
class HomeSimulator {
public:
    HomeSimulator();

    [[nodiscard]] Snapshot snapshot() const { return m_snapshot; }

    // Applies a remote client's command to the simulated home. Returns true
    // if it actually changed anything (an unknown command, an unknown
    // room/lock target, or a value matching the current state all return
    // false without modifying the snapshot).
    bool applyCommand(const Command& command);

    // Not part of the client-facing command protocol - the server uses this
    // to drive the simulated sensor blips independent of any client
    // command.
    void setSensorTriggered(int index, bool triggered);

private:
    [[nodiscard]] int roomIndex(const QString& name) const;
    [[nodiscard]] int lockIndex(const QString& name) const;

    Snapshot m_snapshot;
};

} // namespace qttutorial::homeautomation::remote
