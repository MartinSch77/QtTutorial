// SPDX-License-Identifier: MIT
#include "TelemetrySimulator.h"

namespace qttutorial::space {

namespace {
constexpr int kTickIntervalMs = 200;
constexpr double kTickIntervalS = kTickIntervalMs / 1000.0;
constexpr double kSimulationSpeedup = 40.0; // compress a ~95 minute orbit into a watchable demo

// A single fictitious fixed ground station, used only to demonstrate a
// station-in-view indicator on the icon strip; not a real facility.
constexpr double kHomeStationLatitudeDeg = 45.0;
constexpr double kHomeStationLongitudeDeg = 10.0;
constexpr double kHomeStationViewRadiusDeg = 35.0;
}

TelemetrySimulator::TelemetrySimulator(QObject* parent)
    : QObject(parent)
{
    m_subsystems.push_back(std::make_unique<SubsystemHealthMachine>(QStringLiteral("Battery"), 30.0, 100.0, 15.0,
                                                                       100.0, this));
    m_subsystems.push_back(std::make_unique<SubsystemHealthMachine>(QStringLiteral("Battery Bay Thermal"), 5.0,
                                                                       25.0, -10.0, 40.0, this));
    m_subsystems.push_back(
        std::make_unique<SubsystemHealthMachine>(QStringLiteral("Payload Thermal"), -20.0, 35.0, -35.0, 50.0, this));
    m_subsystems.push_back(std::make_unique<SubsystemHealthMachine>(QStringLiteral("Avionics Thermal"), 0.0, 30.0,
                                                                       -15.0, 45.0, this));

    connect(&m_timer, &QTimer::timeout, this, &TelemetrySimulator::tick);
}

void TelemetrySimulator::start()
{
    for (auto& subsystem : m_subsystems) {
        subsystem->start();
    }
    m_timer.start(kTickIntervalMs);
}

bool TelemetrySimulator::homeStationInView() const
{
    return isWithinStationView(m_groundTrack.state(), kHomeStationLatitudeDeg, kHomeStationLongitudeDeg,
                                kHomeStationViewRadiusDeg);
}

void TelemetrySimulator::tick()
{
    const double dtSeconds = kTickIntervalS * kSimulationSpeedup;

    m_orbit.step(dtSeconds);
    m_power.step(dtSeconds, m_orbit.state().solarInputFraction, /*panelsSunPointed=*/true);
    m_thermal.step(dtSeconds, m_orbit.state().solarInputFraction);
    m_groundTrack.step(dtSeconds, m_orbit.state().trueAnomalyDeg);

    m_subsystems[0]->updateValue(m_power.state().batterySocPercent);
    m_subsystems[1]->updateValue(m_thermal.temperatureC(ThermalZone::BatteryBay));
    m_subsystems[2]->updateValue(m_thermal.temperatureC(ThermalZone::Payload));
    m_subsystems[3]->updateValue(m_thermal.temperatureC(ThermalZone::Avionics));

    emit telemetryUpdated();
}

} // namespace qttutorial::space
