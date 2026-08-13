// SPDX-License-Identifier: MIT
#include "HaulTruckTelemetry.h"

namespace qttutorial::mining {

HaulTruckTelemetry::HaulTruckTelemetry(QObject* parent)
    : QObject(parent)
{
    m_state = m_simulator.state();
    m_timer.setInterval(50);
    connect(&m_timer, &QTimer::timeout, this, &HaulTruckTelemetry::tick);
    m_timer.start();
}

void HaulTruckTelemetry::tick()
{
    m_state = m_simulator.advance(0.05);
    emit telemetryChanged();
}

QString HaulTruckTelemetry::haulStateLabel() const
{
    switch (m_state.haulState) {
    case HaulState::Loading:
        return QStringLiteral("LOADING");
    case HaulState::Hauling:
        return QStringLiteral("HAULING");
    case HaulState::Dumping:
        return QStringLiteral("DUMPING");
    case HaulState::Returning:
        return QStringLiteral("RETURNING");
    }
    return QStringLiteral("LOADING");
}

QVariantList HaulTruckTelemetry::tyrePressuresKPa() const
{
    QVariantList pressures;
    for (const double pressure : m_state.tyrePressuresKPa) {
        pressures << pressure;
    }
    return pressures;
}

QVariantList HaulTruckTelemetry::tyreTempsC() const
{
    QVariantList temps;
    for (const double temp : m_state.tyreTempsC) {
        temps << temp;
    }
    return temps;
}

double HaulTruckTelemetry::speedExpectedMinKph() const
{
    return HaulCycleSimulator::expectedSpeedRangeAt(m_state.haulState).minKph;
}

double HaulTruckTelemetry::speedExpectedMaxKph() const
{
    return HaulCycleSimulator::expectedSpeedRangeAt(m_state.haulState).maxKph;
}

bool HaulTruckTelemetry::speedOutOfRange() const
{
    const SpeedRangeKph range = HaulCycleSimulator::expectedSpeedRangeAt(m_state.haulState);
    return m_state.speedKph < range.minKph - 1.0 || m_state.speedKph > range.maxKph + 1.0;
}

} // namespace qttutorial::mining
