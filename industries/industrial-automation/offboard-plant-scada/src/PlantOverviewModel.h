// SPDX-License-Identifier: MIT
#pragma once

#include "AlarmEvaluator.h"
#include "ProcessPointSimulator.h"

#include <QAbstractListModel>
#include <QString>

#include <vector>

namespace qttutorial::plant_scada {

// A snapshot of one plant line/cell for the plant-wide overview: whether its
// gated equipment is currently running, and the worst alarm severity among
// all of its tags' latest readings.
struct LineStatus {
    QString line;
    bool running = false;
    Severity worstSeverity = Severity::Normal;
};

// Pure aggregation, independently testable without a GUI: groups the given
// tags by TagDefinition::line and rolls each line's latest values up into a
// LineStatus. `tags` and `latestValues` must be the same size and index-
// aligned (as TagTableModel keeps them). A line counts as running if any of
// its gatedByLineRunning tags reads above a small "equipment is actually
// doing something" threshold.
[[nodiscard]] std::vector<LineStatus> summarizeLines(const std::vector<TagDefinition>& tags,
                                                       const std::vector<double>& latestValues);

// A thin QAbstractListModel wrapper around summarizeLines(), so the plant
// overview widget can bind to it the same way TagTableModel exposes tags.
class PlantOverviewModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Role { LineNameRole = Qt::UserRole + 1, RunningRole, SeverityRole };

    explicit PlantOverviewModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void update(const std::vector<TagDefinition>& tags, const std::vector<double>& latestValues);
    [[nodiscard]] const std::vector<LineStatus>& lines() const { return m_lines; }

private:
    std::vector<LineStatus> m_lines;
};

} // namespace qttutorial::plant_scada
