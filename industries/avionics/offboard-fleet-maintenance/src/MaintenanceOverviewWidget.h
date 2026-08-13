// SPDX-License-Identifier: MIT
#pragma once

#include "InspectionScheduler.h"

#include <QString>
#include <QWidget>

#include <vector>

namespace qttutorial::avionics {

struct FleetOverviewEntry {
    QString tailNumber;
    InspectionStatus status;
};

// A fleet-wide "maintenance due" overview: one row per aircraft, each drawn as
// a bar showing flight-hours-remaining-to-inspection (as a fraction of the
// scheduled interval) plus the cycles-remaining count, sorted most-urgent
// first. Urgent rows get a procedurally-drawn caution-triangle glyph (built
// from QPainterPath primitives, no external icon asset) so the most
// operationally important aircraft are visually obvious to a non-technical
// viewer at a glance, the way a real ops-centre "big board" would present it.
class MaintenanceOverviewWidget : public QWidget {
    Q_OBJECT
public:
    explicit MaintenanceOverviewWidget(QWidget* parent = nullptr);

    void setEntries(std::vector<FleetOverviewEntry> entries);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    std::vector<FleetOverviewEntry> m_entries;
};

} // namespace qttutorial::avionics
