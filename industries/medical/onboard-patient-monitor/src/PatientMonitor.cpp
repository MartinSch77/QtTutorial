// SPDX-License-Identifier: MIT
#include "PatientMonitor.h"

#include "EcgWaveformGenerator.h"
#include "RespirationWaveformGenerator.h"
#include "VitalsSimulator.h"

namespace qttutorial::medical {

namespace {
constexpr int kEcgBufferSize = 200;
constexpr int kRespirationBufferSize = 200;
constexpr double kTickSeconds = 0.04;
}

PatientMonitor::PatientMonitor(QObject* parent)
    : QObject(parent)
{
    m_ecgSamples.reserve(kEcgBufferSize);
    for (int i = 0; i < kEcgBufferSize; ++i) {
        m_ecgSamples.append(0.0);
    }
    m_respirationSamples.reserve(kRespirationBufferSize);
    for (int i = 0; i < kRespirationBufferSize; ++i) {
        m_respirationSamples.append(0.0);
    }

    m_timer.setInterval(static_cast<int>(kTickSeconds * 1000));
    connect(&m_timer, &QTimer::timeout, this, &PatientMonitor::tick);
    m_timer.start();
}

void PatientMonitor::tick()
{
    m_timeSeconds += kTickSeconds;

    m_heartRate = VitalsSimulator::heartRateAt(m_timeSeconds);
    m_spo2 = VitalsSimulator::spo2At(m_timeSeconds);
    m_systolic = VitalsSimulator::systolicAt(m_timeSeconds);
    m_diastolic = VitalsSimulator::diastolicAt(m_timeSeconds);
    m_respirationRate = VitalsSimulator::respirationRateAt(m_timeSeconds);
    m_temperature = VitalsSimulator::temperatureAt(m_timeSeconds);
    m_alarmLevel = m_alarmMachine.update(m_heartRate, m_spo2);

    m_ecgSamples.removeFirst();
    m_ecgSamples.append(EcgWaveformGenerator::sample(m_timeSeconds, m_heartRate));

    m_respirationSamples.removeFirst();
    m_respirationSamples.append(RespirationWaveformGenerator::sample(m_timeSeconds, m_respirationRate));

    emit vitalsChanged();
}

QString PatientMonitor::alarmLevelText() const
{
    switch (m_alarmLevel) {
    case AlarmLevel::Normal:
        return QStringLiteral("Normal");
    case AlarmLevel::Warning:
        return QStringLiteral("Caution");
    case AlarmLevel::Critical:
        return QStringLiteral("Critical");
    }
    return QStringLiteral("Normal");
}

} // namespace qttutorial::medical
