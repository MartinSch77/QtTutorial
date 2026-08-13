// SPDX-License-Identifier: MIT
#include "BummerlScore.h"

namespace qttutorial::watten {

namespace {
std::size_t indexOf(Team team)
{
    return team == Team::TeamA ? 0 : 1;
}
} // namespace

void BummerlScore::addHandWin(Team team)
{
    if (isWon()) {
        return;
    }
    ++m_score[indexOf(team)];
}

int BummerlScore::score(Team team) const
{
    return m_score[indexOf(team)];
}

bool BummerlScore::isWon() const
{
    return m_score[0] >= kTarget || m_score[1] >= kTarget;
}

std::optional<Team> BummerlScore::winner() const
{
    if (m_score[0] >= kTarget) {
        return Team::TeamA;
    }
    if (m_score[1] >= kTarget) {
        return Team::TeamB;
    }
    return std::nullopt;
}

} // namespace qttutorial::watten
