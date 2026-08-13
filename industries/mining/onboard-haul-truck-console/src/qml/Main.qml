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
        spacing: 16

        Row {
            width: parent.width
            spacing: 10

            HaulTruckIcon {
                width: 44
                height: 30
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("HAUL TRUCK MT-014 - CAB CONSOLE")
                color: "#f2f4f8"
                font.bold: true
                font.pixelSize: 18
            }

            Item { width: 20; height: 1 }

            PitIcon {
                width: 28
                height: 28
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Bench 4 - North Pit")
                color: "#9aa4b2"
                font.pixelSize: 13
            }
        }

        HaulStateStrip {
            anchors.horizontalCenter: parent.horizontalCenter
            stateIndex: telemetry.haulStateIndex
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 50

            Column {
                spacing: 4
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 6
                    PayloadScaleIcon {
                        width: 16
                        height: 16
                        glyphColor: telemetry.overloaded ? "#e5484d" : "#3ddc6f"
                    }
                    Text {
                        text: qsTr("PAYLOAD")
                        color: "#9aa4b2"
                        font.pixelSize: 11
                        font.bold: true
                    }
                }
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

            Gauge {
                width: 220
                height: 220
                value: telemetry.speedKph
                minValue: 0
                maxValue: 70
                bandMinValue: telemetry.speedExpectedMinKph
                bandMaxValue: telemetry.speedExpectedMaxKph
                label: qsTr("speed km/h")
                accentColor: "#39c0ff"
                alarm: telemetry.speedOutOfRange
            }

            Gauge {
                width: 180
                height: 180
                value: telemetry.fuelLtrPerHour
                minValue: 0
                maxValue: 200
                label: qsTr("fuel L/h")
                accentColor: "#e5b93d"
                alarm: false
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 12

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("TYRES")
                color: "#9aa4b2"
                font.pixelSize: 12
                font.bold: true
            }

            TyrePressureGrid {
                anchors.verticalCenter: parent.verticalCenter
                pressuresKPa: telemetry.tyrePressuresKPa
                tempsC: telemetry.tyreTempsC
            }
        }
    }

    Rectangle {
        id: overloadBanner
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 44
        color: "#c0392b"
        visible: telemetry.overloaded

        Row {
            anchors.centerIn: parent
            spacing: 10

            WarningTriangleIcon {
                width: 26
                height: 24
                anchors.verticalCenter: parent.verticalCenter
                triangleColor: "#ffe8a3"
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
                font.bold: true
                text: qsTr("WARNING: payload exceeds rated capacity of %1 t").arg(telemetry.ratedCapacityTonnes)
            }
        }
    }

    Rectangle {
        id: speedOutOfRangeBanner
        anchors.top: overloadBanner.visible ? overloadBanner.bottom : parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 36
        color: "#8a5a12"
        visible: telemetry.speedOutOfRange && !telemetry.overloaded

        Row {
            anchors.centerIn: parent
            spacing: 10

            WarningTriangleIcon {
                width: 22
                height: 20
                anchors.verticalCenter: parent.verticalCenter
                triangleColor: "#ffe8a3"
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
                font.bold: true
                text: qsTr("ADVISORY: speed outside expected range for %1 (%2-%3 km/h)")
                    .arg(telemetry.haulStateLabel)
                    .arg(Math.round(telemetry.speedExpectedMinKph))
                    .arg(Math.round(telemetry.speedExpectedMaxKph))
            }
        }
    }
}
