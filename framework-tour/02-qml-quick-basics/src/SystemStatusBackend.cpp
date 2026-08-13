// SPDX-License-Identifier: MIT
#include "SystemStatusBackend.h"

namespace qttutorial::qml_basics {

SystemStatusBackend::SystemStatusBackend(QObject* parent)
    : QObject(parent)
    , m_indicators(new IndicatorListModel(this))
{
    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &SystemStatusBackend::advance);
    m_timer.start();
}

IndicatorListModel* SystemStatusBackend::indicators() const
{
    return m_indicators;
}

QString SystemStatusBackend::overallStatus() const
{
    return m_overallStatus;
}

int SystemStatusBackend::uptimeSeconds() const
{
    return m_tick;
}

void SystemStatusBackend::advance()
{
    ++m_tick;
    m_indicators->refresh(m_tick);

    const auto readings = IndicatorGenerator::generate(m_tick);
    QString worst = QStringLiteral("nominal");
    for (const auto& reading : readings) {
        if (reading.status == IndicatorStatus::Critical) {
            worst = QStringLiteral("critical");
            break;
        }
        if (reading.status == IndicatorStatus::Warning) {
            worst = QStringLiteral("warning");
        }
    }
    if (worst != m_overallStatus) {
        m_overallStatus = worst;
        emit overallStatusChanged();
    }
    emit uptimeSecondsChanged();
}

} // namespace qttutorial::qml_basics
