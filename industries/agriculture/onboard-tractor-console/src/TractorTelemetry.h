// SPDX-License-Identifier: MIT
#pragma once

#include "FieldPassSimulator.h"

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QTimer>

namespace qttutorial::agriculture {

// Thin QObject/QML façade over FieldPassSimulator: owns the timer that steps the
// simulation and republishes its FieldPassState as Q_PROPERTYs the in-cab console
// QML can bind to. All the actual field-pass logic lives in FieldPassSimulator so
// it can be unit tested without QML or an event loop.
class TractorTelemetry : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int passNumber READ passNumber NOTIFY telemetryChanged)
    Q_PROPERTY(double coveragePercent READ coveragePercent NOTIFY telemetryChanged)
    Q_PROPERTY(double crossTrackErrorCm READ crossTrackErrorCm NOTIFY telemetryChanged)
    Q_PROPERTY(double workingDepthCm READ workingDepthCm NOTIFY telemetryChanged)
    Q_PROPERTY(bool implementEngaged READ implementEngaged NOTIFY telemetryChanged)
    Q_PROPERTY(double engineLoadPercent READ engineLoadPercent NOTIFY telemetryChanged)
    Q_PROPERTY(double fuelLevelPercent READ fuelLevelPercent NOTIFY telemetryChanged)
    Q_PROPERTY(double yieldRateTonsPerHour READ yieldRateTonsPerHour NOTIFY telemetryChanged)
    Q_PROPERTY(int rowIndex READ rowIndex NOTIFY telemetryChanged)
    Q_PROPERTY(int fieldRowCount READ fieldRowCount CONSTANT)
    Q_PROPERTY(bool movingForward READ movingForward NOTIFY telemetryChanged)
    Q_PROPERTY(QString implementKind READ implementKind NOTIFY telemetryChanged)
public:
    explicit TractorTelemetry(QObject* parent = nullptr);

    [[nodiscard]] int passNumber() const { return m_state.passNumber; }
    [[nodiscard]] double coveragePercent() const { return m_state.coveragePercent; }
    [[nodiscard]] double crossTrackErrorCm() const { return m_state.crossTrackErrorCm; }
    [[nodiscard]] double workingDepthCm() const { return m_state.workingDepthCm; }
    [[nodiscard]] bool implementEngaged() const { return m_state.implementEngaged; }
    [[nodiscard]] double engineLoadPercent() const { return m_state.engineLoadPercent; }
    [[nodiscard]] double fuelLevelPercent() const { return m_state.fuelLevelPercent; }
    [[nodiscard]] double yieldRateTonsPerHour() const { return m_state.yieldRateTonsPerHour; }
    [[nodiscard]] int rowIndex() const { return m_state.rowIndex; }
    [[nodiscard]] int fieldRowCount() const { return FieldPassSimulator::kFieldRowCount; }
    [[nodiscard]] bool movingForward() const { return m_state.movingForward; }
    [[nodiscard]] QString implementKind() const;

signals:
    void telemetryChanged();

private:
    void tick();

    FieldPassSimulator m_simulator;
    FieldPassState m_state;
    QTimer m_timer;
};

} // namespace qttutorial::agriculture
