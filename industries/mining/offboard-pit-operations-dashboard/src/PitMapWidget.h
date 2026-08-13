// SPDX-License-Identifier: MIT
#pragma once

#include "HaulFleetSimulator.h"

#include <QWidget>

#include <vector>

namespace qttutorial::mining::pit {

// A hand-painted, top-down pit-overview panel: a generic open-pit motif
// (concentric benches) with each truck rendered as a small haul-truck glyph
// at its current simulated position, coloured by haul-cycle phase. This is
// not a real mapping/GIS widget - just a QPainter sketch of "where is
// everyone right now" for a control-room overview, in the spirit of an
// autonomous-haulage fleet-management dashboard.
class PitMapWidget : public QWidget {
    Q_OBJECT
public:
    explicit PitMapWidget(QWidget* parent = nullptr);

    void setSamples(const std::vector<TruckSample>& samples);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    [[nodiscard]] static QColor colorForState(int stateIndex);

    std::vector<TruckSample> m_samples;
};

} // namespace qttutorial::mining::pit
