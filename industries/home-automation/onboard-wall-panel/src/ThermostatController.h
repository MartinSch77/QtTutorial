// SPDX-License-Identifier: MIT
#pragma once

#include "SceneRegistry.h"
#include "ThermostatModel.h"

#include <QObject>
#include <QQmlEngine>
#include <QTimer>

namespace qttutorial::homeautomation {

// QObject/QML façade: steps the simulated thermal model on a QTimer and
// republishes it as Q_PROPERTYs. All of the actual thermal simulation lives
// in ThermostatModel, which has no Qt GUI dependency and is unit tested
// independent of this class.
class ThermostatController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(double currentTemperature READ currentTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(double targetTemperature READ targetTemperature WRITE setTargetTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY temperatureChanged)
public:
    enum class Mode { Off, Heat, Cool };
    Q_ENUM(Mode)

    explicit ThermostatController(QObject* parent = nullptr);

    [[nodiscard]] double currentTemperature() const { return m_model.currentTemperature(); }
    [[nodiscard]] double targetTemperature() const { return m_model.targetTemperature(); }
    void setTargetTemperature(double celsius);
    [[nodiscard]] Mode mode() const { return static_cast<Mode>(m_model.mode()); }
    void setMode(Mode mode);

    // Moves the target/mode straight to the scene's setpoint; the current
    // temperature still drifts there gradually via the regular tick(), so a
    // scene change is felt over time rather than snapping instantly.
    Q_INVOKABLE void applyScene(int sceneId);

signals:
    void temperatureChanged();

private:
    void tick();

    ThermostatModel m_model;
    QTimer m_timer;
};

} // namespace qttutorial::homeautomation
