// SPDX-License-Identifier: MIT
#pragma once

#include "AnnunciatorLogic.h"
#include "FlightDataSimulator.h"

#include <QWidget>

class QLabel;

namespace qttutorial::avionics {

class AttitudeIndicator;
class VerticalTape;
class HeadingIndicator;
class VerticalSpeedIndicator;
class CompassRose;
class AnnunciatorPanel;

// A fullscreen, kiosk-style embedded HMI: the Primary Flight Display. No network,
// no persistence, driven purely by the on-timer flight dynamics simulation.
class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void onStateChanged(const FlightState& state);

    FlightDataSimulator m_simulator;
    AnnunciatorLogic m_annunciatorLogic;
    AttitudeIndicator* m_attitude;
    VerticalTape* m_airspeedTape;
    VerticalTape* m_altitudeTape;
    HeadingIndicator* m_heading;
    VerticalSpeedIndicator* m_verticalSpeed;
    CompassRose* m_compassRose;
    AnnunciatorPanel* m_annunciatorPanel;
    QLabel* m_statusLabel;
};

} // namespace qttutorial::avionics
