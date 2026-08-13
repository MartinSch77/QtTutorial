// SPDX-License-Identifier: MIT
#pragma once

#include "RouteProfile.h"

#include <QObject>
#include <QTimer>

namespace qttutorial::cab_display {

enum class SignalAspect { Green, Yellow, Red };

// Advances a train along a RouteProfile with physically consistent motion:
// speed and position are integrated, not assigned, and the permitted speed
// shown to the driver is derived from the same braking-curve function used
// for signal supervision on real ETCS-fitted lines.
class TrainSimulator : public QObject {
    Q_OBJECT
    Q_PROPERTY(double speedKmh READ speedKmh NOTIFY updated)
    Q_PROPERTY(double permittedSpeedKmh READ permittedSpeedKmh NOTIFY updated)
    Q_PROPERTY(double distanceToRestrictionM READ distanceToRestrictionM NOTIFY updated)
    Q_PROPERTY(double restrictionSpeedKmh READ restrictionSpeedKmh NOTIFY updated)
    Q_PROPERTY(bool doorsOpen READ doorsOpen NOTIFY updated)
    Q_PROPERTY(int signalAspectIndex READ signalAspectIndex NOTIFY updated)
    Q_PROPERTY(QString nextStationName READ nextStationName NOTIFY updated)
    Q_PROPERTY(double nextStationDistanceM READ nextStationDistanceM NOTIFY updated)
    Q_PROPERTY(double positionM READ positionM NOTIFY updated)

public:
    static constexpr double kMaxAccelerationMs2 = 0.6;
    static constexpr double kServiceDecelerationMs2 = 0.8;
    static constexpr double kStationStopToleranceM = 15.0;
    static constexpr double kDwellSeconds = 8.0;

    explicit TrainSimulator(RouteProfile route, QObject* parent = nullptr);

    [[nodiscard]] double positionM() const { return m_positionM; }
    [[nodiscard]] double speedKmh() const { return m_speedKmh; }
    [[nodiscard]] double permittedSpeedKmh() const { return m_permittedSpeedKmh; }
    [[nodiscard]] double distanceToRestrictionM() const { return m_distanceToRestrictionM; }
    [[nodiscard]] double restrictionSpeedKmh() const { return m_restrictionSpeedKmh; }
    [[nodiscard]] bool doorsOpen() const { return m_doorsOpen; }
    [[nodiscard]] SignalAspect signalAspect() const { return m_signalAspect; }
    [[nodiscard]] int signalAspectIndex() const { return static_cast<int>(m_signalAspect); }
    [[nodiscard]] QString nextStationName() const { return m_nextStationName; }
    [[nodiscard]] double nextStationDistanceM() const { return m_nextStationDistanceM; }

public slots:
    // Pure(ish) simulation step: advances the train by dtSeconds. Only
    // reads/writes member state and the route profile, so tests can drive
    // it directly without a timer or event loop.
    void tick(double dtSeconds);

signals:
    void updated();

private:
    RouteProfile m_route;
    double m_positionM = 0.0;
    double m_speedKmh = 0.0;
    double m_permittedSpeedKmh = 0.0;
    double m_distanceToRestrictionM = 0.0;
    double m_restrictionSpeedKmh = 0.0;
    bool m_doorsOpen = false;
    double m_dwellRemainingSeconds = 0.0;
    SignalAspect m_signalAspect = SignalAspect::Green;
    QString m_nextStationName;
    double m_nextStationDistanceM = 0.0;
    QTimer m_timer;
};

} // namespace qttutorial::cab_display
