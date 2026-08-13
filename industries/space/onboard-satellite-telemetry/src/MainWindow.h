// SPDX-License-Identifier: MIT
#pragma once

#include "TelemetrySimulator.h"

#include <QSet>
#include <QString>
#include <QWidget>

class QLabel;

namespace qttutorial::space {

class RadialGauge;
class SubsystemHealthGrid;
class StatusIconStrip;

// A fullscreen, kiosk-style embedded HMI for onboard satellite telemetry: no
// network, no persistence, driven purely by the on-timer orbital/power/thermal
// simulation and a small bank of Qt State Machine subsystem health monitors.
class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void onTelemetryUpdated();
    void onSubsystemHealthChanged(const QString& subsystemName, const QString& stateName);

    TelemetrySimulator m_simulator;
    QSet<QString> m_alertingSubsystems;
    QLabel* m_orbitLabel;
    QLabel* m_eclipseLabel;
    QLabel* m_groundTrackLabel;
    StatusIconStrip* m_iconStrip;
    RadialGauge* m_batteryGauge;
    RadialGauge* m_radiatorGauge;
    RadialGauge* m_batteryBayGauge;
    RadialGauge* m_payloadGauge;
    RadialGauge* m_avionicsGauge;
    SubsystemHealthGrid* m_healthGrid;
};

} // namespace qttutorial::space
