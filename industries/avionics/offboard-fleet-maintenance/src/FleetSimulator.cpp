// SPDX-License-Identifier: MIT
#include "FleetSimulator.h"

#include <algorithm>

namespace qttutorial::avionics {

namespace {
constexpr double kVibrationNominal = 0.15;
constexpr double kVibrationCritical = 0.45;
}

FleetSimulator::FleetSimulator(QObject* parent)
    : QObject(parent)
{
    seedFleet();
}

void FleetSimulator::seedFleet()
{
    const struct { const char* tail; const char* type; } fleet[] = {
        {"N101QT", "A320"},
        {"N102QT", "A320"},
        {"N103QT", "B737"},
        {"N104QT", "B737"},
        {"N105QT", "E190"},
        {"N106QT", "A320"},
    };

    m_aircraft.clear();
    m_vibrationTrends.clear();
    m_vibrationDriftPerSecond.clear();

    int index = 0;
    for (const auto& entry : fleet) {
        Aircraft aircraft;
        aircraft.tailNumber = QString::fromLatin1(entry.tail);
        aircraft.type = QString::fromLatin1(entry.type);
        m_aircraft.push_back(aircraft);
        m_vibrationTrends.emplace_back(0.0, kVibrationCritical, 24);

        // Two aircraft in the fleet get a slow, believable drift fault so the
        // predictive-maintenance flag has something genuine to detect.
        const double drift = (index == 1 || index == 4) ? 0.0009 : 0.0;
        m_vibrationDriftPerSecond.push_back(drift);
        ++index;
    }
}

double FleetSimulator::stepParameter(double current, double nominal, double driftPerSecond, double dtSeconds,
                                      std::normal_distribution<double>& noise)
{
    const double pullToNominal = (nominal - current) * 0.02 * dtSeconds;
    const double drift = driftPerSecond * dtSeconds;
    return current + pullToNominal + drift + noise(m_rng) * dtSeconds;
}

void FleetSimulator::advance(double dtSeconds)
{
    std::normal_distribution<double> vibrationNoise(0.0, 0.01);
    std::normal_distribution<double> egtNoise(0.0, 0.3);
    std::normal_distribution<double> oilNoise(0.0, 0.2);

    for (std::size_t i = 0; i < m_aircraft.size(); ++i) {
        Aircraft& aircraft = m_aircraft[i];

        aircraft.engine1.vibrationIps = std::max(
            0.0, stepParameter(aircraft.engine1.vibrationIps, kVibrationNominal, m_vibrationDriftPerSecond[i],
                                dtSeconds, vibrationNoise));
        aircraft.engine1.egtMarginC =
            std::max(0.0, stepParameter(aircraft.engine1.egtMarginC, 40.0, 0.0, dtSeconds, egtNoise));
        aircraft.engine1.oilPressurePsi =
            std::max(0.0, stepParameter(aircraft.engine1.oilPressurePsi, 65.0, 0.0, dtSeconds, oilNoise));

        aircraft.engine2.vibrationIps = std::max(
            0.0, stepParameter(aircraft.engine2.vibrationIps, kVibrationNominal, 0.0, dtSeconds, vibrationNoise));
        aircraft.engine2.egtMarginC =
            std::max(0.0, stepParameter(aircraft.engine2.egtMarginC, 40.0, 0.0, dtSeconds, egtNoise));
        aircraft.engine2.oilPressurePsi =
            std::max(0.0, stepParameter(aircraft.engine2.oilPressurePsi, 65.0, 0.0, dtSeconds, oilNoise));

        m_vibrationTrends[i].addSample(aircraft.engine1.vibrationIps);
        const bool wasFlagged = aircraft.maintenanceFlag;
        aircraft.maintenanceFlag = m_vibrationTrends[i].isDrifting();

        if (aircraft.maintenanceFlag && !wasFlagged) {
            MaintenanceTask task;
            task.id = QStringLiteral("MX-%1").arg(m_nextTaskId++);
            task.aircraftTail = aircraft.tailNumber;
            task.description = QStringLiteral("Engine 1 vibration trending out of nominal band - inspect");
            task.priority = QStringLiteral("Elevated");
            m_tasks.push_back(task);
            emit maintenanceTaskCreated(task);
        }
    }

    emit fleetUpdated();
}

} // namespace qttutorial::avionics
