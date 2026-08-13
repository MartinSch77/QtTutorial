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
    width: 1440
    height: 760
    visible: true
    color: "#12151b"
    title: qsTr("QtTutorial - Tractor Console")

    TractorTelemetry {
        id: telemetry
    }

    Column {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        // Header bar: in the style of a Gen4-class in-cab display's status
        // strip, identifying the machine, the mounted implement and the
        // current field position at a glance.
        Rectangle {
            width: parent.width
            height: 56
            radius: 8
            color: "#1c212b"

            Row {
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                spacing: 12

                FarmIcon {
                    width: 32
                    height: 32
                    kind: "tractor"
                    color: "#3ddc6f"
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("FIELD OPS CONSOLE")
                    color: "#f2f4f8"
                    font.pixelSize: 18
                    font.bold: true
                }
            }

            Row {
                anchors.right: parent.right
                anchors.rightMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                spacing: 24

                Row {
                    spacing: 8
                    anchors.verticalCenter: parent.verticalCenter
                    FarmIcon {
                        width: 26
                        height: 26
                        kind: "field"
                        color: "#9aa4b2"
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTr("pass %1 · row %2/%3").arg(telemetry.passNumber).arg(telemetry.rowIndex + 1).arg(telemetry.fieldRowCount)
                        color: "#9aa4b2"
                        font.pixelSize: 14
                    }
                }

                Row {
                    spacing: 8
                    anchors.verticalCenter: parent.verticalCenter
                    FarmIcon {
                        width: 26
                        height: 26
                        kind: telemetry.implementKind
                        color: telemetry.implementEngaged ? "#3ddc6f" : "#e5b93d"
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: telemetry.implementEngaged ? qsTr("IMPLEMENT ENGAGED") : qsTr("IMPLEMENT RAISED")
                        color: telemetry.implementEngaged ? "#3ddc6f" : "#e5b93d"
                        font.pixelSize: 14
                        font.bold: true
                    }
                }
            }
        }

        CrossTrackBar {
            width: parent.width
            value: telemetry.crossTrackErrorCm
        }

        Row {
            width: parent.width
            spacing: 40

            Column {
                spacing: 18
                Gauge {
                    width: 220
                    height: 220
                    value: telemetry.fuelLevelPercent
                    minValue: 0
                    maxValue: 100
                    label: qsTr("fuel")
                    unit: "%"
                    accentColor: "#3ddc6f"
                }

                Gauge {
                    width: 220
                    height: 220
                    value: telemetry.yieldRateTonsPerHour
                    minValue: 0
                    maxValue: 50
                    label: qsTr("yield rate")
                    unit: " t/h"
                    accentColor: "#e5b93d"
                }
            }

            // Field-coverage map: the centrepiece "map view" panel, tracking
            // the boustrophedon pass pattern across the whole field rather
            // than just the current pass in isolation.
            Column {
                spacing: 12
                width: 460

                Rectangle {
                    width: parent.width
                    height: 90
                    radius: 8
                    color: "#1c212b"

                    Row {
                        anchors.centerIn: parent
                        spacing: 16

                        FarmIcon {
                            width: 34
                            height: 34
                            kind: telemetry.implementKind
                            color: telemetry.implementEngaged ? "#3ddc6f" : "#e5b93d"
                        }

                        Column {
                            spacing: 4
                            Text {
                                text: telemetry.implementEngaged ? qsTr("IMPLEMENT ENGAGED") : qsTr("IMPLEMENT RAISED")
                                color: telemetry.implementEngaged ? "#3ddc6f" : "#e5b93d"
                                font.pixelSize: 16
                                font.bold: true
                            }
                            Text {
                                text: qsTr("depth: %1 cm  ·  %2").arg(telemetry.workingDepthCm.toFixed(1))
                                    .arg(telemetry.movingForward ? qsTr("heading out") : qsTr("heading back"))
                                color: "#9aa4b2"
                                font.pixelSize: 13
                            }
                        }
                    }
                }

                FieldCoverageMap {
                    width: parent.width
                    height: 260
                    coveragePercent: telemetry.coveragePercent
                    rowIndex: telemetry.rowIndex
                    fieldRowCount: telemetry.fieldRowCount
                    movingForward: telemetry.movingForward
                }
            }

            Column {
                spacing: 18

                Gauge {
                    width: 220
                    height: 220
                    value: telemetry.engineLoadPercent
                    minValue: 0
                    maxValue: 100
                    label: qsTr("engine load")
                    unit: "%"
                    accentColor: "#ff9f43"
                }

                CoverageArc {
                    width: 220
                    height: 220
                    percent: telemetry.coveragePercent
                    passNumber: telemetry.passNumber
                }
            }
        }
    }
}
