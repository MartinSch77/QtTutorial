// SPDX-License-Identifier: MIT
#include "VehicleTelemetry.h"

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

} // namespace qttutorial::automotive
