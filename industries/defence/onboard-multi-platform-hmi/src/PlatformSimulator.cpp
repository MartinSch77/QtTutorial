// SPDX-License-Identifier: MIT
#include "PlatformSimulator.h"

#include <QVariantMap>

#include <algorithm>
#include <cmath>

namespace qttutorial::defence {

namespace {
constexpr int kTickIntervalMs = 150;
}

PlatformSimulator::PlatformSimulator(QObject* parent)
    : QObject(parent)
{
    const struct { const char* name; double nominalLow; double nominalHigh; double criticalLow; double criticalHigh; } bands[] = {
        {"Propulsion", 30.0, 100.0, 15.0, 100.0},
        {"Power", 40.0, 100.0, 20.0, 100.0},
        {"Comms", 50.0, 100.0, 25.0, 100.0},
        {"Sensors", 45.0, 100.0, 20.0, 100.0},
    };

    for (const auto& band : bands) {
        auto machine = std::make_unique<SubsystemHealthMachine>(QString::fromLatin1(band.name), band.nominalLow,
                                                                  band.nominalHigh, band.criticalLow,
                                                                  band.criticalHigh, this);
        connect(machine.get(), &SubsystemHealthMachine::healthChanged, this,
                &PlatformSimulator::onSubsystemHealthChanged);
        QVariantMap entry;
        entry[QStringLiteral("name")] = QString::fromLatin1(band.name);
        entry[QStringLiteral("state")] = QStringLiteral("Nominal");
        entry[QStringLiteral("value")] = band.nominalHigh;
        m_subsystems.push_back(entry);
        machine->start();
        m_subsystemMachines.push_back(std::move(machine));
    }

    connect(&m_timer, &QTimer::timeout, this, &PlatformSimulator::tick);
    m_timer.start(kTickIntervalMs);
}

void PlatformSimulator::tick()
{
    const double dtSeconds = kTickIntervalMs / 1000.0;
    m_elapsedSeconds += dtSeconds;

    m_trackSimulator.advance(dtSeconds);
    m_vehicleStatus.advance(dtSeconds);
    m_detectionSimulator.advance(dtSeconds);
    m_hudSimulator.advance(dtSeconds);

    const double propulsionValue = 70.0 + 25.0 * std::sin(m_elapsedSeconds * 0.05);
    const double powerValue = 80.0 - 15.0 * std::max(0.0, std::sin(m_elapsedSeconds * 0.03));
    // The Comms subsystem occasionally dips hard (rather than gently
    // oscillating like the others) so the correlated tactical-data-link
    // staleness effect below has something visible to demonstrate.
    const double commsValue =
        60.0 + 35.0 * std::sin(m_elapsedSeconds * 0.11) - 25.0 * std::max(0.0, std::sin(m_elapsedSeconds * 0.02));
    const double sensorsValue = 85.0 + 10.0 * std::sin(m_elapsedSeconds * 0.07);

    m_subsystemMachines[0]->updateValue(propulsionValue);
    m_subsystemMachines[1]->updateValue(powerValue);
    m_subsystemMachines[2]->updateValue(commsValue);
    m_subsystemMachines[3]->updateValue(sensorsValue);

    const double values[] = {propulsionValue, powerValue, commsValue, sensorsValue};
    for (int i = 0; i < static_cast<int>(m_subsystems.size()) && i < 4; ++i) {
        QVariantMap map = m_subsystems[i].toMap();
        map[QStringLiteral("value")] = std::clamp(values[i], 0.0, 100.0);
        m_subsystems[i] = map;
    }

    // Comms link quality (clamped to a displayable 0-100 percentage) drives
    // both the Comms health-grid cell above and the tactical data-link
    // track staleness below - the same simulated cause, two correlated,
    // purely informational effects.
    m_commsQualityPercent = std::clamp(commsValue, 0.0, 100.0);
    m_dataLinkModel.advance(dtSeconds, m_commsQualityPercent);

    emit dataChanged();
}

void PlatformSimulator::onSubsystemHealthChanged(const QString& subsystemName, const QString& stateName)
{
    for (QVariant& entry : m_subsystems) {
        QVariantMap map = entry.toMap();
        if (map.value(QStringLiteral("name")).toString() == subsystemName) {
            map[QStringLiteral("state")] = stateName;
            entry = map;
            break;
        }
    }
}

QVariantList PlatformSimulator::tracks() const
{
    QVariantList list;
    for (const Track& track : m_trackSimulator.tracks()) {
        QVariantMap map;
        map[QStringLiteral("id")] = track.id;
        map[QStringLiteral("classification")] = track.classification;
        map[QStringLiteral("domain")] = track.domain;
        map[QStringLiteral("xKm")] = track.xKm;
        map[QStringLiteral("yKm")] = track.yKm;
        map[QStringLiteral("headingDeg")] = track.headingDeg;
        map[QStringLiteral("speedKmh")] = track.speedKmh;
        list.push_back(map);
    }
    return list;
}

QVariantList PlatformSimulator::detections() const
{
    QVariantList list;
    for (const DetectedObject& object : m_detectionSimulator.detections()) {
        QVariantMap map;
        map[QStringLiteral("id")] = object.id;
        map[QStringLiteral("label")] = object.label;
        map[QStringLiteral("confidence")] = object.confidence;
        map[QStringLiteral("x")] = object.x;
        map[QStringLiteral("y")] = object.y;
        map[QStringLiteral("w")] = object.w;
        map[QStringLiteral("h")] = object.h;
        list.push_back(map);
    }
    return list;
}

QVariantList PlatformSimulator::waypoints() const
{
    QVariantList list;
    for (const HudWaypoint& waypoint : m_hudSimulator.waypoints()) {
        QVariantMap map;
        map[QStringLiteral("label")] = waypoint.label;
        map[QStringLiteral("bearingDeg")] = waypoint.bearingDeg;
        map[QStringLiteral("distanceKm")] = waypoint.distanceKm;
        list.push_back(map);
    }
    return list;
}

QVariantList PlatformSimulator::teammates() const
{
    QVariantList list;
    for (const HudTeammate& teammate : m_hudSimulator.teammates()) {
        QVariantMap map;
        map[QStringLiteral("id")] = teammate.id;
        map[QStringLiteral("bearingDeg")] = teammate.bearingDeg;
        map[QStringLiteral("distanceKm")] = teammate.distanceKm;
        list.push_back(map);
    }
    return list;
}

QVariantList PlatformSimulator::dataLinkTracks() const
{
    QVariantList list;
    for (const DataLinkTrack& track : m_dataLinkModel.tracks()) {
        QVariantMap map;
        map[QStringLiteral("id")] = track.id;
        map[QStringLiteral("classification")] = track.classification;
        map[QStringLiteral("domain")] = track.domain;
        map[QStringLiteral("xKm")] = track.xKm;
        map[QStringLiteral("yKm")] = track.yKm;
        map[QStringLiteral("headingDeg")] = track.headingDeg;
        map[QStringLiteral("speedKmh")] = track.speedKmh;
        map[QStringLiteral("stale")] = track.stale;
        map[QStringLiteral("dataAgeSeconds")] = track.dataAgeSeconds;
        list.push_back(map);
    }
    return list;
}

} // namespace qttutorial::defence
