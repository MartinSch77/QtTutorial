// SPDX-License-Identifier: GPL-3.0-or-later
#include "TelemetryFeed.h"

#include "TelemetryGenerator.h"

namespace qttutorial::graphs_showcase {

TelemetryFeed::TelemetryFeed(QObject* parent)
    : QObject(parent)
{
    m_timer.setInterval(static_cast<int>(kTickSeconds * 1000.0));
    connect(&m_timer, &QTimer::timeout, this, &TelemetryFeed::advance);
    m_timer.start();
}

QStringList TelemetryFeed::sensorNames() const
{
    QStringList names;
    for (const auto& name : TelemetryGenerator::sensorNames()) {
        names.push_back(name);
    }
    return names;
}

void TelemetryFeed::advance()
{
    m_elapsedSeconds += kTickSeconds;
    emit samplesReady(m_elapsedSeconds, TelemetryGenerator::valueAt(0, m_elapsedSeconds),
                       TelemetryGenerator::valueAt(1, m_elapsedSeconds),
                       TelemetryGenerator::valueAt(2, m_elapsedSeconds));
}

} // namespace qttutorial::graphs_showcase
