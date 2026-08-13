// SPDX-License-Identifier: MIT
#include "PlantOverviewModel.h"

#include <algorithm>
#include <cmath>
#include <iterator>

namespace qttutorial::plant_scada {

namespace {
// A gated tag (flow/conveyor/motor-speed style) reading above this fraction
// of its base value counts as "the equipment is actually moving product",
// distinguishing it from the small residual valueAt() returns while stopped.
constexpr double kRunningFraction = 0.2;
}

std::vector<LineStatus> summarizeLines(const std::vector<TagDefinition>& tags, const std::vector<double>& latestValues)
{
    std::vector<LineStatus> lines;
    const std::size_t count = std::min(tags.size(), latestValues.size());
    for (std::size_t i = 0; i < count; ++i) {
        const TagDefinition& tag = tags[i];
        auto it = std::find_if(lines.begin(), lines.end(), [&](const LineStatus& s) { return s.line == tag.line; });
        if (it == lines.end()) {
            lines.push_back(LineStatus{tag.line, false, Severity::Normal});
            it = std::prev(lines.end());
        }

        const double value = latestValues[i];
        const Severity severity = evaluate(tag, value);
        if (severity > it->worstSeverity) {
            it->worstSeverity = severity;
        }
        if (tag.gatedByLineRunning && std::abs(value) >= std::abs(tag.baseValue) * kRunningFraction) {
            it->running = true;
        }
    }
    return lines;
}

PlantOverviewModel::PlantOverviewModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int PlantOverviewModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_lines.size());
}

QVariant PlantOverviewModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_lines.size())) {
        return {};
    }
    const LineStatus& status = m_lines[static_cast<std::size_t>(index.row())];
    switch (role) {
    case LineNameRole:
        return status.line;
    case RunningRole:
        return status.running;
    case SeverityRole:
        return static_cast<int>(status.worstSeverity);
    default:
        return {};
    }
}

QHash<int, QByteArray> PlantOverviewModel::roleNames() const
{
    return {
        {LineNameRole, "lineName"},
        {RunningRole, "running"},
        {SeverityRole, "severity"},
    };
}

void PlantOverviewModel::update(const std::vector<TagDefinition>& tags, const std::vector<double>& latestValues)
{
    beginResetModel();
    m_lines = summarizeLines(tags, latestValues);
    endResetModel();
}

} // namespace qttutorial::plant_scada
