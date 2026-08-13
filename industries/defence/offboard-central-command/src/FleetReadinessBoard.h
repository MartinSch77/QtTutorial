// SPDX-License-Identifier: MIT
#pragma once

#include "FleetReadiness.h"

#include <QWidget>

namespace qttutorial::defence {

// A simple green/amber/red fleet-readiness summary board: three tiles with
// a procedurally-drawn shield glyph and a count, computed from
// summarizeReadiness(). Passive status roll-up only.
class FleetReadinessBoard : public QWidget {
    Q_OBJECT
public:
    explicit FleetReadinessBoard(QWidget* parent = nullptr);

    void setSummary(FleetReadinessSummary summary);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    FleetReadinessSummary m_summary;
};

} // namespace qttutorial::defence
