// SPDX-License-Identifier: MIT
#include "TractorTelemetry.h"

namespace qttutorial::agriculture {

TractorTelemetry::TractorTelemetry(QObject* parent)
    : QObject(parent)
{
    m_state = m_simulator.state();
    m_timer.setInterval(50);
    connect(&m_timer, &QTimer::timeout, this, &TractorTelemetry::tick);
    m_timer.start();
}

void TractorTelemetry::tick()
{
    m_state = m_simulator.advance(0.05);
    emit telemetryChanged();
}

QString TractorTelemetry::implementKind() const
{
    switch (m_state.implementKind) {
    case ImplementKind::Plow:
        return QStringLiteral("plow");
    case ImplementKind::Sprayer:
        return QStringLiteral("sprayer");
    case ImplementKind::Planter:
    default:
        return QStringLiteral("planter");
    }
}

} // namespace qttutorial::agriculture
