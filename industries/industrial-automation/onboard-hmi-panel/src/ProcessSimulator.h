// SPDX-License-Identifier: MIT
#pragma once

#include <QObject>
#include <QTimer>

namespace qttutorial::industrial_hmi {

// Simulates a single tank/pump/pipe loop with believable first-order dynamics:
// the tank level integrates the difference between inflow and outflow rather
// than jumping to a target, and pressure follows flow with a short lag. tick()
// is the pure, testable step function; the constructor additionally arms a
// QTimer that drives it in real time for the running application.
class ProcessSimulator : public QObject {
    Q_OBJECT
    Q_PROPERTY(double tankLevel READ tankLevel NOTIFY tankLevelChanged)
    Q_PROPERTY(double flowRate READ flowRate NOTIFY flowRateChanged)
    Q_PROPERTY(double pressure READ pressure NOTIFY pressureChanged)
    Q_PROPERTY(bool pumpRunning READ isPumpRunning NOTIFY pumpRunningChanged)

public:
    static constexpr double kTankCapacityPercent = 100.0;
    static constexpr double kMaxInflowRate = 8.0;
    static constexpr double kOutflowRate = 5.0;
    static constexpr double kHighLevelAlarm = 90.0;
    static constexpr double kLowLevelAlarm = 10.0;
    static constexpr double kHighPressureAlarm = 7.5;

    explicit ProcessSimulator(QObject* parent = nullptr);

    [[nodiscard]] double tankLevel() const { return m_tankLevel; }
    [[nodiscard]] double flowRate() const { return m_flowRate; }
    [[nodiscard]] double pressure() const { return m_pressure; }
    [[nodiscard]] bool isPumpRunning() const { return m_pumpRunning; }

public slots:
    void setPumpRunning(bool running);
    // Advances the simulation by dtSeconds. Pure and deterministic, so tests can
    // drive it directly without an event loop or real time passing.
    void tick(double dtSeconds);

signals:
    void tankLevelChanged(double value);
    void flowRateChanged(double value);
    void pressureChanged(double value);
    void pumpRunningChanged(bool running);
    void highLevelAlarm();
    void lowLevelAlarm();
    void highPressureAlarm();

private:
    double m_tankLevel = 45.0;
    double m_flowRate = 0.0;
    double m_pressure = 0.0;
    bool m_pumpRunning = false;
    QTimer m_timer;
};

} // namespace qttutorial::industrial_hmi
