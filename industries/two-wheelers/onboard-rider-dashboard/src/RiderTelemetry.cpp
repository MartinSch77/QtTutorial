// SPDX-License-Identifier: MIT
#include "RiderTelemetry.h"

namespace qttutorial::two_wheelers {

RiderTelemetry::RiderTelemetry(QObject* parent)
    : QObject(parent)
{
    m_state = m_simulator.state();
    m_timer.setInterval(50);
    connect(&m_timer, &QTimer::timeout, this, &RiderTelemetry::tick);
    m_timer.start();
}

void RiderTelemetry::tick()
{
    m_state = m_simulator.advance(0.05);
    emit telemetryChanged();
}

QString RiderTelemetry::gearLabel() const
{
    switch (m_state.gear) {
    case Gear::Neutral:
        return QStringLiteral("N");
    case Gear::Gear1:
        return QStringLiteral("1");
    case Gear::Gear2:
        return QStringLiteral("2");
    case Gear::Gear3:
        return QStringLiteral("3");
    case Gear::Gear4:
        return QStringLiteral("4");
    case Gear::Gear5:
        return QStringLiteral("5");
    case Gear::Gear6:
        return QStringLiteral("6");
    }
    return QStringLiteral("N");
}

} // namespace qttutorial::two_wheelers
