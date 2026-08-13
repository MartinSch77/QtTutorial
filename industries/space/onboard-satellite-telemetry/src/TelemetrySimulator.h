// SPDX-License-Identifier: MIT
#pragma once

#include "GroundTrackSimulator.h"
#include "OrbitalSimulator.h"
#include "PowerSystem.h"
#include "SubsystemHealthMachine.h"
#include "ThermalSimulator.h"

#include <QObject>
#include <QTimer>

#include <memory>
#include <vector>

namespace qttutorial::space {

// Ties the orbital, power and thermal simulators together and drives a small
// bank of SubsystemHealthMachine instances (one per subsystem) from their
// respective parameters, on a QTimer. This is the top-level onboard telemetry
// producer the UI subscribes to.
class TelemetrySimulator : public QObject {
    Q_OBJECT
public:
    explicit TelemetrySimulator(QObject* parent = nullptr);

    void start();

    [[nodiscard]] const OrbitalState& orbitalState() const { return m_orbit.state(); }
    [[nodiscard]] const PowerState& powerState() const { return m_power.state(); }
    [[nodiscard]] double thermalZoneTemperatureC(ThermalZone zone) const { return m_thermal.temperatureC(zone); }
    [[nodiscard]] const GroundTrackState& groundTrackState() const { return m_groundTrack.state(); }
    [[nodiscard]] bool homeStationInView() const;
    [[nodiscard]] const std::vector<std::unique_ptr<SubsystemHealthMachine>>& subsystemMachines() const
    {
        return m_subsystems;
    }

signals:
    void telemetryUpdated();

private:
    void tick();

    OrbitalSimulator m_orbit;
    PowerSystem m_power;
    ThermalSimulator m_thermal;
    GroundTrackSimulator m_groundTrack;
    std::vector<std::unique_ptr<SubsystemHealthMachine>> m_subsystems;
    QTimer m_timer;
};

} // namespace qttutorial::space
