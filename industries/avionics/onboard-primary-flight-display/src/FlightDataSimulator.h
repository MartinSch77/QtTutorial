// SPDX-License-Identifier: MIT
#pragma once

#include "FlightDynamics.h"

#include <QObject>
#include <QTimer>

namespace qttutorial::avionics {

// Drives FlightDynamics on a QTimer with a scripted "autopilot" control input
// (gentle sinusoidal turns and pitch changes), so the PFD demo shows believable,
// continuously coupled instrument motion without needing real control hardware.
class FlightDataSimulator : public QObject {
    Q_OBJECT
public:
    explicit FlightDataSimulator(QObject* parent = nullptr);

    void start();
    void stop();

    [[nodiscard]] const FlightState& state() const { return m_dynamics.state(); }

signals:
    void stateChanged(const FlightState& state);

private:
    void tick();

    FlightDynamics m_dynamics;
    QTimer m_timer;
    double m_elapsedSeconds = 0.0;
};

} // namespace qttutorial::avionics
