// SPDX-License-Identifier: MIT
#include "FleetSimulator.h"

#include <algorithm>

namespace qttutorial::avionics {

namespace {
constexpr double kVibrationNominal = 0.15;
constexpr double kVibrationCritical = 0.45;

// Demo-accelerated utilization rates: real aircraft accrue hours/cycles over
// days of operation, but a technology showcase needs the fleet-wide
// maintenance-due overview to visibly move within a short demo session.
constexpr double kFlightHoursPerSecond = 0.03;
constexpr double kCyclesPerSecond = 0.015;
}

FleetSimulator::FleetSimulator(QObject* parent)
    : QObject(parent)
{
    seedFleet();
}

void FleetSimulator::seedFleet()
{
    // hoursSeed/cyclesSeed are deliberately chosen so the two aircraft with a
    // seeded vibration-drift fault (index 1 and 4, below) are *also* close to
    // their next scheduled inspection: that is the concrete correlation this
    // simulator demonstrates - an aircraft that is both due soon and already
    // showing an engine-parameter anomaly should be flagged more urgently than
    // one that is merely due soon, which InspectionScheduler expresses.
    const struct {
        const char* tail;
        const char* type;
        double hoursSeed;
        int cyclesSeed;
    } fleet[] = {
        {"N101QT", "A320", 50.0, 80},
        {"N102QT", "A320", 370.0, 560},
        {"N103QT", "B737", 120.0, 200},
        {"N104QT", "B737", 210.0, 340},
        {"N105QT", "E190", 380.0, 575},
        {"N106QT", "A320", 300.0, 450},
    };

    m_aircraft.clear();
    m_vibrationTrends.clear();
    m_vibrationDriftPerSecond.clear();
    m_wasUrgent.clear();
    m_cyclesFractional.clear();

    int index = 0;
    for (const auto& entry : fleet) {
        Aircraft aircraft;
        aircraft.tailNumber = QString::fromLatin1(entry.tail);
        aircraft.type = QString::fromLatin1(entry.type);
        aircraft.flightHoursSinceInspection = entry.hoursSeed;
        aircraft.cyclesSinceInspection = entry.cyclesSeed;
        m_aircraft.push_back(aircraft);
        m_vibrationTrends.emplace_back(0.0, kVibrationCritical, 24);
        m_wasUrgent.push_back(false);
        m_cyclesFractional.push_back(0.0);

        // Two aircraft in the fleet get a directional drift fault so the
        // predictive-maintenance flag has something genuine to detect. The
        // random-walk pulls each parameter back toward nominal at a rate of
        // 0.02/second, so a one-directional drift settles into a steady-state
        // offset of drift/0.02 above nominal; this drift is picked so that
        // steady-state offset (about 0.30 ips) actually pushes the value
        // through the critical band during the transient, rather than
        // settling into an offset that never leaves the nominal band - a
        // real drift fault should eventually be *caught*, not just nudge the
        // parameter a little.
        const double drift = (index == 1 || index == 4) ? 0.008 : 0.0;
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

        // Accumulate utilization since last inspection (demo-accelerated -
        // see kFlightHoursPerSecond/kCyclesPerSecond above).
        aircraft.flightHoursSinceInspection += kFlightHoursPerSecond * dtSeconds;
        m_cyclesFractional[i] += kCyclesPerSecond * dtSeconds;
        while (m_cyclesFractional[i] >= 1.0) {
            aircraft.cyclesSinceInspection += 1;
            m_cyclesFractional[i] -= 1.0;
        }

        // Correlate utilization with the engine-anomaly flag: an aircraft
        // that becomes urgent (either overdue outright, or due soon *and*
        // currently showing an active anomaly) gets an escalated task, once,
        // on the transition into that state.
        const InspectionStatus inspection =
            m_inspectionScheduler.evaluate(aircraft.flightHoursSinceInspection, aircraft.cyclesSinceInspection,
                                            aircraft.maintenanceFlag);
        const bool isUrgentNow = inspection.urgency == InspectionUrgency::Urgent;
        if (isUrgentNow && !m_wasUrgent[i]) {
            MaintenanceTask task;
            task.id = QStringLiteral("MX-%1").arg(m_nextTaskId++);
            task.aircraftTail = aircraft.tailNumber;
            task.description = aircraft.maintenanceFlag
                ? QStringLiteral("Inspection due soon and engine anomaly active - prioritize")
                : QStringLiteral("Scheduled inspection interval reached");
            task.priority = QStringLiteral("Urgent");
            m_tasks.push_back(task);
            emit maintenanceTaskCreated(task);
        }
        m_wasUrgent[i] = isUrgentNow;
    }

    emit fleetUpdated();
}

InspectionStatus FleetSimulator::inspectionStatus(std::size_t aircraftIndex) const
{
    if (aircraftIndex >= m_aircraft.size()) {
        return {};
    }
    const Aircraft& aircraft = m_aircraft[aircraftIndex];
    return m_inspectionScheduler.evaluate(aircraft.flightHoursSinceInspection, aircraft.cyclesSinceInspection,
                                           aircraft.maintenanceFlag);
}

} // namespace qttutorial::avionics
