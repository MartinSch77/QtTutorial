// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window
import QtTutorial.Agriculture.Onboard

// Kiosk-style in-cab console for a tractor/combine. In a real embedded
// deployment this window would be shown with visibility: Window.FullScreen and
// no window decorations; it is left windowed here so the demo runs on a
// regular desktop.
Window {
    id: window
    width: 1280
    height: 520
    visible: true
    color: "#12151b"
    title: qsTr("QtTutorial - Tractor Console")

    TractorTelemetry {
        id: telemetry
    }

    Column {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 20

        CrossTrackBar {
            width: parent.width
            value: telemetry.crossTrackErrorCm
        }

        Row {
            width: parent.width
            spacing: 60

            Gauge {
                width: 240
                height: 240
                value: telemetry.fuelLevelPercent
                minValue: 0
                maxValue: 100
                label: qsTr("fuel")
                unit: "%"
                accentColor: "#3ddc6f"
            }

            Column {
                spacing: 18
                width: 260

                Rectangle {
                    width: parent.width
                    height: 90
                    radius: 8
                    color: "#1c212b"

                    Column {
                        anchors.centerIn: parent
                        spacing: 4

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: telemetry.implementEngaged ? qsTr("IMPLEMENT ENGAGED") : qsTr("IMPLEMENT RAISED")
                            color: telemetry.implementEngaged ? "#3ddc6f" : "#e5b93d"
                            font.pixelSize: 16
                            font.bold: true
                        }
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTr("depth: %1 cm").arg(telemetry.workingDepthCm.toFixed(1))
                            color: "#9aa4b2"
                            font.pixelSize: 13
                        }
                    }
                }

                CoverageArc {
                    width: 220
                    height: 220
                    anchors.horizontalCenter: parent.horizontalCenter
                    percent: telemetry.coveragePercent
                    passNumber: telemetry.passNumber
                }
            }

            Gauge {
                width: 240
                height: 240
                value: telemetry.engineLoadPercent
                minValue: 0
                maxValue: 100
                label: qsTr("engine load")
                unit: "%"
                accentColor: "#ff9f43"
            }
        }
    }
}
