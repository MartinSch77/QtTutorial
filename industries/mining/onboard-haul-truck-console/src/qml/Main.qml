// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window
import QtTutorial.Mining.Onboard

// Kiosk-style haul-truck operator console. In a real deployment this window
// would be shown with visibility: Window.FullScreen and no window
// decorations; it is left windowed here so the demo runs on a regular desktop.
Window {
    id: window
    width: 1280
    height: 560
    visible: true
    color: "#12151b"
    title: qsTr("QtTutorial - Haul Truck Console")

    HaulTruckTelemetry {
        id: telemetry
    }

    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        HaulStateStrip {
            anchors.horizontalCenter: parent.horizontalCenter
            stateIndex: telemetry.haulStateIndex
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 50

            Gauge {
                width: 280
                height: 280
                value: telemetry.payloadTonnes
                minValue: 0
                maxValue: 350
                label: qsTr("payload t")
                accentColor: "#3ddc6f"
                alarm: telemetry.overloaded
            }

            Gauge {
                width: 220
                height: 220
                value: telemetry.engineTempC
                minValue: 0
                maxValue: 140
                label: qsTr("engine °C")
                accentColor: "#ff9f43"
                alarm: telemetry.engineTempC > 110
            }

            Gauge {
                width: 220
                height: 220
                value: telemetry.retarderTempC
                minValue: 0
                maxValue: 140
                label: qsTr("retarder °C")
                accentColor: "#39c0ff"
                alarm: telemetry.retarderTempC > 100
            }

            TyrePressureGrid {
                anchors.verticalCenter: parent.verticalCenter
                pressuresKPa: telemetry.tyrePressuresKPa
            }
        }
    }

    Rectangle {
        id: overloadBanner
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        color: "#c0392b"
        visible: telemetry.overloaded

        Text {
            anchors.centerIn: parent
            color: "white"
            font.bold: true
            text: qsTr("WARNING: payload exceeds rated capacity of %1 t").arg(telemetry.ratedCapacityTonnes)
        }
    }
}
