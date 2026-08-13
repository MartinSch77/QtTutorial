// SPDX-License-Identifier: MIT
#pragma once

#include "FleetSimulator.h"

#include <QWidget>

namespace qttutorial::fleet_ops {

// A schematic fleet map: the shared loop route drawn as a circle (a stylised
// track diagram, the kind an operations-centre wallboard shows, not a
// geographic map), station ticks around it, and one train marker per live
// train, coloured by punctuality and drawn as a small procedurally-drawn
// train icon rather than a plain dot. Positions come from the same
// `TrainState::positionKm` the fleet table already shows, via the pure
// `positionOnLoop` geometry function, so the map and the table can never
// disagree about where a train is.
class FleetMapWidget : public QWidget {
    Q_OBJECT
public:
    explicit FleetMapWidget(QWidget* parent = nullptr);

    void setFleet(const FleetConfig& config, const std::vector<TrainState>& states);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    FleetConfig m_config;
    std::vector<TrainState> m_states;
};

} // namespace qttutorial::fleet_ops
