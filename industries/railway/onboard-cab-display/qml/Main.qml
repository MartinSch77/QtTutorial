// SPDX-License-Identifier: MIT
//
// Design reference: this cab display's visual language is inspired by the
// genre of Alstom/Siemens-style ETCS Driver Machine Interfaces (a circular
// speed dial with a target-speed marker, a signal-aspect lamp, a
// distance-to-target readout, and a "planning area" strip of upcoming speed
// restrictions). It is a style/genre reference only: no trademark, logo,
// wordmark, exact colour scheme, or layout of any specific vendor's product
// is reproduced, and no external image/icon assets are used — every icon
// here is drawn procedurally on QML Canvas.
import QtQuick
import QtQuick.Window

Window {
    id: window
    visible: true
    width: 1180
    height: 480
    minimumWidth: 960
    minimumHeight: 420
    title: "Cab Display"
    color: "#05070a"

    readonly property var aspectNames: ["GREEN", "YELLOW", "RED"]
    readonly property real speedoMax: 160
    readonly property bool overSpeed: train.speedKmh > train.permittedSpeedKmh + 0.5

    Row {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 24

        // --- Speed dial -----------------------------------------------
        Item {
            id: speedo
            width: 340
            height: parent.height

            Rectangle {
                anchors.fill: parent
                radius: 12
                color: "#0d1420"
                border.color: overSpeed ? "#e5484d" : "#243044"
                border.width: overSpeed ? 2 : 1
            }

            SpeedDial {
                id: dial
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 12
                width: parent.width - 24
                height: width
                speedKmh: train.speedKmh
                permittedSpeedKmh: train.permittedSpeedKmh
                maxSpeedKmh: speedoMax
                warning: overSpeed
            }

            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: dial.bottom
                anchors.topMargin: -dial.height * 0.22
                spacing: 2

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: train.speedKmh.toFixed(0)
                    color: overSpeed ? "#e5484d" : "#f2f4f7"
                    font.pixelSize: 40
                    font.bold: true
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "km/h  ·  permitted " + train.permittedSpeedKmh.toFixed(0)
                    color: "#8a94a3"
                    font.pixelSize: 12
                }
            }
        }

        Column {
            width: parent.width - speedo.width - 24
            height: parent.height
            spacing: 14

            // --- Brake warning banner ------------------------------------
            // Always present (so the layout below doesn't jump when it
            // activates); only its colouring, icon and text react to state.
            Rectangle {
                id: brakeBanner
                width: parent.width
                height: 36
                radius: 8
                color: train.brakeWarningActive ? "#2a1216" : "#0d1420"
                border.color: train.brakeWarningActive ? "#e5484d" : "#243044"
                opacity: train.brakeWarningActive ? pulse.value : 1.0

                SequentialAnimation {
                    id: pulse
                    property real value: 1.0
                    running: train.brakeWarningActive
                    loops: Animation.Infinite
                    NumberAnimation { target: pulse; property: "value"; from: 1.0; to: 0.55; duration: 400 }
                    NumberAnimation { target: pulse; property: "value"; from: 0.55; to: 1.0; duration: 400 }
                }

                Row {
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 8
                    BrakeIcon { width: 22; height: 22; active: train.brakeWarningActive }
                    Text {
                        text: train.brakeWarningActive ? "OVER SPEED — BRAKE" : "SPEED SUPERVISION NORMAL"
                        color: train.brakeWarningActive ? "#e5484d" : "#5a6576"
                        font.pixelSize: 14
                        font.bold: train.brakeWarningActive
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }

            // --- Margin / next station ---------------------------------
            Rectangle {
                width: parent.width
                height: 66
                radius: 10
                color: "#0d1420"
                border.color: "#243044"

                Row {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 20

                    Row {
                        spacing: 8
                        RailIcon { width: 16; height: 30; anchors.verticalCenter: parent.verticalCenter }
                        Column {
                            anchors.verticalCenter: parent.verticalCenter
                            Text { text: "MARGIN TO NEXT RESTRICTION"; color: "#8a94a3"; font.pixelSize: 10 }
                            Text {
                                text: Number.isFinite(train.distanceToRestrictionM)
                                      ? train.distanceToRestrictionM.toFixed(0) + " m -> " + train.restrictionSpeedKmh.toFixed(0) + " km/h"
                                      : "clear"
                                color: "#f2f4f7"
                                font.pixelSize: 17
                                font.bold: true
                            }
                        }
                    }

                    Rectangle { width: 1; height: parent.height; color: "#243044" }

                    Row {
                        spacing: 8
                        TrainIcon { width: 20; height: 26; anchors.verticalCenter: parent.verticalCenter }
                        Column {
                            anchors.verticalCenter: parent.verticalCenter
                            Text { text: "NEXT STATION"; color: "#8a94a3"; font.pixelSize: 10 }
                            Text {
                                text: train.nextStationName + " · " + train.nextStationDistanceM.toFixed(0) + " m"
                                color: "#f2f4f7"
                                font.pixelSize: 17
                                font.bold: true
                            }
                        }
                    }
                }
            }

            // --- Signal aspect / doors ----------------------------------
            Row {
                width: parent.width
                spacing: 16

                Rectangle {
                    width: 168
                    height: 96
                    radius: 10
                    color: "#0d1420"
                    border.color: "#243044"

                    Row {
                        anchors.centerIn: parent
                        spacing: 10
                        SignalLampIcon {
                            width: 30
                            height: 66
                            aspectIndex: train.signalAspectIndex
                        }
                        Column {
                            anchors.verticalCenter: parent.verticalCenter
                            Text { text: "SIGNAL"; color: "#8a94a3"; font.pixelSize: 10 }
                            Text {
                                text: aspectNames[train.signalAspectIndex]
                                color: "#f2f4f7"
                                font.pixelSize: 15
                                font.bold: true
                            }
                        }
                    }
                }

                Rectangle {
                    width: 168
                    height: 96
                    radius: 10
                    color: "#0d1420"
                    border.color: train.doorsOpen ? "#e5b93d" : "#243044"

                    Row {
                        anchors.centerIn: parent
                        spacing: 10
                        DoorsIcon {
                            width: 40
                            height: 50
                            open: train.doorsOpen
                            color: train.doorsOpen ? "#e5b93d" : "#3ddc84"
                        }
                        Column {
                            anchors.verticalCenter: parent.verticalCenter
                            Text { text: "DOORS"; color: "#8a94a3"; font.pixelSize: 10 }
                            Text {
                                text: train.doorsOpen ? "OPEN" : "CLOSED"
                                color: train.doorsOpen ? "#e5b93d" : "#3ddc84"
                                font.pixelSize: 15
                                font.bold: true
                            }
                        }
                    }
                }

                Rectangle {
                    width: parent.width - 168 - 168 - 32
                    height: 96
                    radius: 10
                    color: "#0d1420"
                    border.color: "#243044"

                    Text {
                        anchors.centerIn: parent
                        text: "Position on route: " + train.positionM.toFixed(0) + " m"
                        color: "#8a94a3"
                        font.pixelSize: 13
                    }
                }
            }

            // --- Planning area -------------------------------------------
            Rectangle {
                width: parent.width
                height: parent.height - 36 - 66 - 96 - 3 * 14
                radius: 10
                color: "#0d1420"
                border.color: "#243044"

                Column {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 8

                    Text { text: "PLANNING AREA — UPCOMING RESTRICTIONS"; color: "#8a94a3"; font.pixelSize: 10 }

                    PlanningStrip {
                        restrictions: train.upcomingRestrictions
                    }
                }
            }
        }
    }
}
