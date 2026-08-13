// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window
import QtTutorial.TwoWheelers.Onboard

// Kiosk-style digital motorcycle dashboard. In a real embedded deployment this
// window would be shown with visibility: Window.FullScreen and no window
// decorations; it is left windowed here so the demo runs on a regular desktop.
//
// Design reference: the overall layout (circular speed/rev gauges either side of
// a centre stack, a lean-angle horizon, a riding-mode selector) takes its visual
// language from the genre of premium sport-bike TFT dashboards and companion
// apps (e.g. Ducati Panigale-style TFT clusters, KTM's connected-ride apps) -
// style/genre inspiration only. No manufacturer logo, wordmark, colour scheme or
// exact layout is reproduced; every glyph on this screen is drawn from scratch
// with QtQuick Canvas paths. See README.md "Design reference" for details.
Window {
    id: window
    width: 1280
    height: 560
    visible: true
    color: "#12151b"
    title: qsTr("QtTutorial - Rider Dashboard")

    readonly property var modeColors: ["#39c0ff", "#5ad46a", "#ff9f43", "#ff4d4d"]
    readonly property color accentColor: modeColors[Math.max(0, Math.min(3, telemetry.ridingModeIndex))]

    RiderTelemetry {
        id: telemetry
    }

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        // Header: generic rider/vehicle glyphs plus the riding-mode selector.
        Row {
            width: parent.width
            height: 44
            spacing: 16

            MotorcycleIcon {
                width: 60
                height: 44
                strokeColor: window.accentColor
            }

            HelmetIcon {
                width: 40
                height: 40
                anchors.verticalCenter: parent.verticalCenter
                strokeColor: "#9aa4b2"
            }

            Item { width: 20; height: 1 }

            RidingModeSelector {
                width: 280
                height: 44
                currentModeIndex: telemetry.ridingModeIndex
                onModeSelected: (modeIndex) => telemetry.setRidingMode(modeIndex)
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 50

            Gauge {
                width: 260
                height: 260
                value: telemetry.speedKph
                minValue: 0
                maxValue: 200
                label: qsTr("km/h")
                accentColor: window.accentColor
            }

            Column {
                spacing: 16
                width: 200

                GearBadge {
                    width: parent.width
                    height: 70
                    gearLabel: telemetry.gearLabel
                    accentColor: window.accentColor
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

                FuelPanel {
                    width: parent.width
                    height: 60
                    fuelLitres: telemetry.fuelLitres
                    fuelPercent: telemetry.fuelPercent
                }
            }

            Gauge {
                width: 260
                height: 260
                value: telemetry.rpm
                minValue: 0
                maxValue: 11000
                label: qsTr("rpm")
                accentColor: window.accentColor
                redlineStart: 8500 / 11000
            }
        }
    }
}
