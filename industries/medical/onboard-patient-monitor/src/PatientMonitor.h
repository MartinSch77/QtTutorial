// SPDX-License-Identifier: MIT
#pragma once

#include "AlarmStateMachine.h"

#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QVariantList>

namespace qttutorial::medical {

// QObject/QML façade: steps the simulated vitals and ECG waveform on a QTimer
// and republishes them as Q_PROPERTYs. All of the actual physiological model
// and alarm logic lives in VitalsSimulator/EcgWaveformGenerator/
// AlarmStateMachine, which have no Qt GUI dependency and are unit tested
// headlessly.
class PatientMonitor : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(double heartRate READ heartRate NOTIFY vitalsChanged)
    Q_PROPERTY(double spo2 READ spo2 NOTIFY vitalsChanged)
    Q_PROPERTY(double systolic READ systolic NOTIFY vitalsChanged)
    Q_PROPERTY(double diastolic READ diastolic NOTIFY vitalsChanged)
    Q_PROPERTY(QString alarmLevelText READ alarmLevelText NOTIFY vitalsChanged)
    Q_PROPERTY(QVariantList ecgSamples READ ecgSamples NOTIFY vitalsChanged)
public:
    explicit PatientMonitor(QObject* parent = nullptr);

    [[nodiscard]] double heartRate() const { return m_heartRate; }
    [[nodiscard]] double spo2() const { return m_spo2; }
    [[nodiscard]] double systolic() const { return m_systolic; }
    [[nodiscard]] double diastolic() const { return m_diastolic; }
    [[nodiscard]] QString alarmLevelText() const;
    [[nodiscard]] QVariantList ecgSamples() const { return m_ecgSamples; }

signals:
    void vitalsChanged();

private:
    void tick();

    QTimer m_timer;
    AlarmStateMachine m_alarmMachine;
    double m_timeSeconds = 0.0;
    double m_heartRate = 72.0;
    double m_spo2 = 97.0;
    double m_systolic = 118.0;
    double m_diastolic = 75.0;
    AlarmLevel m_alarmLevel = AlarmLevel::Normal;
    QVariantList m_ecgSamples;
};

} // namespace qttutorial::medical
