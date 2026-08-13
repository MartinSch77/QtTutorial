// SPDX-License-Identifier: MIT
#pragma once

#include <QDateTime>
#include <QString>

#include <vector>

namespace qttutorial::defence {

enum class AlertSeverity { Info = 0, Caution = 1, Critical = 2 };

struct Alert {
    QString assetId;
    QString message;
    AlertSeverity severity = AlertSeverity::Info;
    QDateTime timestamp;
};

// Keeps an append-only event/alert log and exposes it ordered by severity
// (Critical first), most recent first within the same severity. Pure C++23,
// no Qt dependency, so the ordering logic is unit-testable in isolation.
class AlertLog {
public:
    void addAlert(Alert alert);
    [[nodiscard]] std::vector<Alert> alertsBySeverity() const;
    // Same ordering (Critical first, most recent first within a severity),
    // filtered to only alerts at or above `minimumSeverity` - backs the
    // offboard dashboard's severity filter on the event/alert log.
    [[nodiscard]] std::vector<Alert> alertsBySeverity(AlertSeverity minimumSeverity) const;
    [[nodiscard]] std::size_t size() const { return m_alerts.size(); }

private:
    std::vector<Alert> m_alerts;
};

} // namespace qttutorial::defence
