// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window
import QtTutorial.Automotive.Onboard

// Kiosk-style digital instrument cluster. In a real embedded deployment this
// window would be shown with visibility: Window.FullScreen and no window
// decorations; it is left windowed here so the demo runs on a regular desktop.
Window {
    id: window
    width: 1280
    height: 480
    visible: true
    color: "#12151b"
    title: qsTr("QtTutorial - Instrument Cluster")

    VehicleTelemetry {
        id: telemetry
    }

    Row {
        anchors.centerIn: parent
        spacing: 60

        Gauge {
            width: 300
            height: 300
            value: telemetry.speedKph
            minValue: 0
            maxValue: 220
            label: qsTr("km/h")
            accentColor: "#39c0ff"
        }

        Column {
            spacing: 18
            width: 220

            Rectangle {
                width: parent.width
                height: 90
                radius: 8
                color: "#1c212b"

                Text {
                    anchors.centerIn: parent
                    text: telemetry.gearLabel
                    color: "#f2f4f8"
                    font.pixelSize: 48
                    font.bold: true
                }
            }

            Row {
                spacing: 12
                anchors.horizontalCenter: parent.horizontalCenter

                Text {
                    text: "◀"
                    font.pixelSize: 28
                    color: "#3ddc6f"
                    opacity: telemetry.leftTurnSignal ? 1.0 : 0.15
                }
                Text {
                    text: "▶"
                    font.pixelSize: 28
                    color: "#3ddc6f"
                    opacity: telemetry.rightTurnSignal ? 1.0 : 0.15
                }
            }

            AdasStrip {
                anchors.horizontalCenter: parent.horizontalCenter
                level: telemetry.followingDistanceLevel
            }
        }

        Gauge {
            width: 300
            height: 300
            value: telemetry.rpm
            minValue: 0
            maxValue: 7000
            label: qsTr("rpm")
            accentColor: "#ff9f43"
        }
    }

    Rectangle {
        id: warningBanner
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        color: "#c0392b"
        visible: telemetry.lowFuelWarning || telemetry.laneWarning

        Text {
            anchors.centerIn: parent
            color: "white"
            font.bold: true
            text: telemetry.laneWarning
                  ? qsTr("WARNING: closing distance to traffic ahead")
                  : qsTr("WARNING: low fuel level")
        }
    }
}
