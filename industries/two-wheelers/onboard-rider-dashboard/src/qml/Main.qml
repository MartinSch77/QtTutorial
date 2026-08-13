// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window
import QtTutorial.TwoWheelers.Onboard

// Kiosk-style digital motorcycle dashboard. In a real embedded deployment this
// window would be shown with visibility: Window.FullScreen and no window
// decorations; it is left windowed here so the demo runs on a regular desktop.
Window {
    id: window
    width: 1280
    height: 480
    visible: true
    color: "#12151b"
    title: qsTr("QtTutorial - Rider Dashboard")

    RiderTelemetry {
        id: telemetry
    }

    Row {
        anchors.centerIn: parent
        spacing: 50

        Gauge {
            width: 260
            height: 260
            value: telemetry.speedKph
            minValue: 0
            maxValue: 200
            label: qsTr("km/h")
            accentColor: "#39c0ff"
        }

        Column {
            spacing: 16
            width: 200

            Rectangle {
                width: parent.width
                height: 70
                radius: 8
                color: "#1c212b"

                Text {
                    anchors.centerIn: parent
                    text: telemetry.gearLabel
                    color: "#f2f4f8"
                    font.pixelSize: 40
                    font.bold: true
                }
            }

            LeanIndicator {
                width: parent.width
                height: parent.width
                leanAngleDeg: telemetry.leanAngleDeg
            }

            TyrePanel {
                width: parent.width
                height: 90
                frontPressureBar: telemetry.frontTyrePressureBar
                rearPressureBar: telemetry.rearTyrePressureBar
                frontTempC: telemetry.frontTyreTempC
                rearTempC: telemetry.rearTyreTempC
            }
        }

        Gauge {
            width: 260
            height: 260
            value: telemetry.rpm
            minValue: 0
            maxValue: 11000
            label: qsTr("rpm")
            accentColor: "#ff9f43"
        }
    }
}
