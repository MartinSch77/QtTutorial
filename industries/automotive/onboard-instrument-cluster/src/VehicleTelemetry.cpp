// SPDX-License-Identifier: MIT
#include "VehicleTelemetry.h"

#include <algorithm>

namespace qttutorial::automotive {

VehicleTelemetry::VehicleTelemetry(QObject* parent)
    : QObject(parent)
{
    m_state = m_simulator.state();
    m_timer.setInterval(50);
    connect(&m_timer, &QTimer::timeout, this, &VehicleTelemetry::tick);
    m_timer.start();
}

void VehicleTelemetry::tick()
{
    m_state = m_simulator.advance(0.05);
    emit telemetryChanged();
}

QString VehicleTelemetry::gearLabel() const
{
    switch (m_state.gear) {
    case Gear::Park:
        return QStringLiteral("P");
    case Gear::Reverse:
        return QStringLiteral("R");
    case Gear::Drive1:
        return QStringLiteral("D1");
    case Gear::Drive2:
        return QStringLiteral("D2");
    case Gear::Drive3:
        return QStringLiteral("D3");
    case Gear::Drive4:
        return QStringLiteral("D4");
    case Gear::Drive5:
        return QStringLiteral("D5");
    case Gear::Drive6:
        return QStringLiteral("D6");
    }
    return QStringLiteral("P");
}

void VehicleTelemetry::setDrivingMode(int mode)
{
    const auto clamped = std::clamp(mode, 0, 2);
    const auto newMode = static_cast<DrivingMode>(clamped);
    if (newMode == m_simulator.drivingMode()) {
        return;
    }
    m_simulator.setDrivingMode(newMode);
    m_state.drivingMode = newMode;
    emit telemetryChanged();
}

QString VehicleTelemetry::drivingModeLabel() const
{
    switch (m_state.drivingMode) {
    case DrivingMode::Eco:
        return QStringLiteral("ECO");
    case DrivingMode::Sport:
        return QStringLiteral("SPORT");
    case DrivingMode::Comfort:
    default:
        return QStringLiteral("COMFORT");
    }
}

QColor VehicleTelemetry::drivingModeAccentColor() const
{
    switch (m_state.drivingMode) {
    case DrivingMode::Eco:
        return QColor("#3ddc6f");
    case DrivingMode::Sport:
        return QColor("#ff4b5c");
    case DrivingMode::Comfort:
    default:
        return QColor("#39c0ff");
    }
}

QVariantList VehicleTelemetry::tirePressures() const
{
    QVariantList list;
    list.reserve(static_cast<int>(m_state.tirePressureKpa.size()));
    for (const double pressure : m_state.tirePressureKpa) {
        list.append(pressure);
    }
    return list;
}

} // namespace qttutorial::automotive
