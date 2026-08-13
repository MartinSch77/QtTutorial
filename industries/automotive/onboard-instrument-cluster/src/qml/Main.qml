// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window
import QtTutorial.Automotive.Onboard

// Kiosk-style digital instrument cluster. In a real embedded deployment this
// window would be shown with visibility: Window.FullScreen and no window
// decorations; it is left windowed here so the demo runs on a regular desktop.
//
// Visual language ("Design reference" in README.md): a large glanceable speed
// gauge flanked by secondary readouts, an ambient accent colour that shifts with
// the active driving mode, and a dedicated efficiency readout are inspired by the
// genre of modern digital instrument clusters/hyperscreens (e.g. Mercedes-Benz
// MBUX, Tesla's Model 3 cluster) as a *style* reference only - no trademarked
// logos, wordmarks, or exact layouts are reproduced.
Window {
    id: window
    width: 1400
    height: 620
    visible: true
    color: "#12151b"
    title: qsTr("QtTutorial - Instrument Cluster")

    VehicleTelemetry {
        id: telemetry
    }

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        Row {
            width: parent.width
            height: 34
            spacing: 16

            Text {
                text: qsTr("DRIVE MODE")
                color: "#9aa4b2"
                font.pixelSize: 12
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
            }

            DrivingModeSelector {
                anchors.verticalCenter: parent.verticalCenter
                currentMode: telemetry.drivingMode
                accentColor: telemetry.drivingModeAccentColor
                onModeSelected: function(mode) { telemetry.drivingMode = mode; }
            }

            Item { width: 20; height: 1 }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("EFFICIENCY  %1%").arg(Math.round(telemetry.efficiencyPercent))
                color: telemetry.drivingModeAccentColor
                font.pixelSize: 14
                font.bold: true
            }
        }

        Row {
            width: parent.width
            height: parent.height - 90
            spacing: 40

            FuelGauge {
                anchors.verticalCenter: parent.verticalCenter
                levelPercent: telemetry.fuelLevel
                warning: telemetry.lowFuelWarning
                accentColor: telemetry.drivingModeAccentColor
            }

            Gauge {
                anchors.verticalCenter: parent.verticalCenter
                width: 300
                height: 300
                value: telemetry.speedKph
                minValue: 0
                maxValue: 220
                label: qsTr("km/h")
                accentColor: telemetry.drivingModeAccentColor
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 18
                width: 220

                GearSelector {
                    anchors.horizontalCenter: parent.horizontalCenter
                    gearLabel: telemetry.gearLabel
                    accentColor: telemetry.drivingModeAccentColor
                }

                Row {
                    spacing: 16
                    anchors.horizontalCenter: parent.horizontalCenter

                    TurnSignalArrow {
                        direction: -1
                        active: telemetry.leftTurnSignal
                    }
                    TurnSignalArrow {
                        direction: 1
                        active: telemetry.rightTurnSignal
                    }
                }

                AdasStrip {
                    anchors.horizontalCenter: parent.horizontalCenter
                    level: telemetry.followingDistanceLevel
                }
            }

            Gauge {
                anchors.verticalCenter: parent.verticalCenter
                width: 300
                height: 300
                value: telemetry.rpm
                minValue: 0
                maxValue: 7000
                label: qsTr("rpm")
                accentColor: "#ff9f43"
            }

            TirePressurePanel {
                anchors.verticalCenter: parent.verticalCenter
                pressures: telemetry.tirePressures
                warning: telemetry.tirePressureWarning
                lowWheelIndex: telemetry.lowTireWheelIndex
            }
        }
    }

    Rectangle {
        id: warningBanner
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        color: "#c0392b"
        visible: telemetry.lowFuelWarning || telemetry.laneWarning || telemetry.tirePressureWarning

        Text {
            anchors.centerIn: parent
            color: "white"
            font.bold: true
            text: telemetry.laneWarning
                  ? qsTr("WARNING: closing distance to traffic ahead")
                  : telemetry.tirePressureWarning
                    ? qsTr("WARNING: low tire pressure")
                    : qsTr("WARNING: low fuel level")
        }
    }
}
