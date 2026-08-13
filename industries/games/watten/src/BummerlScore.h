// SPDX-License-Identifier: MIT
#pragma once

#include <QMetaType>

#include <array>
#include <optional>

namespace qttutorial::watten {

enum class Team { TeamA, TeamB };

// Seats 0+2 are TeamA, seats 1+3 are TeamB (partners sit opposite each
// other at the 4-seat table).
[[nodiscard]] constexpr Team teamForSeat(int seat)
{
    return (seat % 2 == 0) ? Team::TeamA : Team::TeamB;
}

// Tracks the running score of a "Bummerl": each won hand is worth exactly
// one point (this implementation's chosen, simplified scoring -- see
// README for the richer scoring schemes real Watten sometimes uses), and
// the Bummerl is won outright by the first team to reach kTarget points.
class BummerlScore {
public:
    static constexpr int kTarget = 11;

    void addHandWin(Team team);
    [[nodiscard]] int score(Team team) const;
    [[nodiscard]] bool isWon() const;
    [[nodiscard]] std::optional<Team> winner() const;

private:
    std::array<int, 2> m_score{0, 0};
};

} // namespace qttutorial::watten

Q_DECLARE_METATYPE(qttutorial::watten::Team)
