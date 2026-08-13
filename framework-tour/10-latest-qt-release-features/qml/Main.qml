// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls.Basic
import QtGraphs
import QtTutorial.LatestQtFeatures

ApplicationWindow {
    id: window
    width: 720
    height: 640
    visible: true
    title: qsTr("Qt Graphs – Live Multi-Sensor Telemetry (Qt 6.9–6.11 showcase)")
    color: "#101418"

    TelemetryFeed {
        id: feed
    }

    property double historySeconds: 20
    property var latestValues: [0, 0, 0]

    Connections {
        target: feed
        function onSamplesReady(elapsedSeconds, sensorA, sensorB, sensorC) {
            seriesA.append(elapsedSeconds, sensorA);
            seriesB.append(elapsedSeconds, sensorB);
            seriesC.append(elapsedSeconds, sensorC);

            const cutoff = elapsedSeconds - historySeconds;
            while (seriesA.count > 0 && seriesA.at(0).x < cutoff) {
                seriesA.remove(0);
                seriesB.remove(0);
                seriesC.remove(0);
            }

            timeAxis.min = Math.max(0, cutoff);
            timeAxis.max = elapsedSeconds;

            latestSet.replace(0, sensorA);
            latestSet.replace(1, sensorB);
            latestSet.replace(2, sensorC);
        }
    }

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        Label {
            text: qsTr("Real-time telemetry (import QtGraphs, Qt Graphs 2D)")
            color: "white"
            font.pixelSize: 16
            font.bold: true
        }

        GraphsView {
            id: lineView
            width: parent.width
            height: parent.height * 0.6

            theme: GraphsTheme {
                colorScheme: GraphsTheme.ColorScheme.Dark
                seriesColors: ["#3ddc84", "#4ac1e0", "#e0a94a"]
            }

            axisX: ValueAxis {
                id: timeAxis
                min: 0
                max: historySeconds
                titleText: qsTr("Elapsed time (s)")
            }
            axisY: ValueAxis {
                min: 0
                max: 100
                titleText: qsTr("Reading")
            }

            LineSeries { id: seriesA; name: qsTr("Sensor A – Pressure") }
            LineSeries { id: seriesB; name: qsTr("Sensor B – Vibration") }
            LineSeries { id: seriesC; name: qsTr("Sensor C – Thermal") }
        }

        Label {
            text: qsTr("Latest readings")
            color: "white"
            font.pixelSize: 16
            font.bold: true
        }

        GraphsView {
            id: barView
            width: parent.width
            height: parent.height * 0.25

            theme: GraphsTheme {
                colorScheme: GraphsTheme.ColorScheme.Dark
                seriesColors: ["#4ac1e0"]
            }

            axisX: BarCategoryAxis {
                categories: [qsTr("Sensor A"), qsTr("Sensor B"), qsTr("Sensor C")]
            }
            axisY: ValueAxis {
                min: 0
                max: 100
            }

            BarSeries {
                BarSet {
                    id: latestSet
                    label: qsTr("Reading")
                    values: [0, 0, 0]
                }
            }
        }
    }
}
