// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QStringList>
#include <QTimer>

namespace qttutorial::graphs_showcase {

// QTimer-driven producer that feeds the live Qt Graphs LineSeries/BarSeries in
// qml/Main.qml. Each tick advances a simulated elapsed-time clock and emits
// one sample per sensor, computed by the pure TelemetryGenerator.
class TelemetryFeed : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QStringList sensorNames READ sensorNames CONSTANT)
public:
    explicit TelemetryFeed(QObject* parent = nullptr);

    [[nodiscard]] QStringList sensorNames() const;

signals:
    void samplesReady(double elapsedSeconds, double sensorA, double sensorB, double sensorC);

private slots:
    void advance();

private:
    QTimer m_timer;
    double m_elapsedSeconds = 0.0;
    static constexpr double kTickSeconds = 0.2;
};

} // namespace qttutorial::graphs_showcase
