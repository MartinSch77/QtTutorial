// SPDX-License-Identifier: MIT
#include "AlertLog.h"

#include <algorithm>

namespace qttutorial::defence {

void AlertLog::addAlert(Alert alert)
{
    m_alerts.push_back(std::move(alert));
}

std::vector<Alert> AlertLog::alertsBySeverity() const
{
    std::vector<Alert> sorted = m_alerts;
    std::stable_sort(sorted.begin(), sorted.end(), [](const Alert& a, const Alert& b) {
        if (a.severity != b.severity) {
            return a.severity > b.severity;
        }
        return a.timestamp > b.timestamp;
    });
    return sorted;
}

} // namespace qttutorial::defence
