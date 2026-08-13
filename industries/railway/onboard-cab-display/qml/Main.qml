// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window

Window {
    id: window
    visible: true
    width: 1000
    height: 420
    minimumWidth: 800
    minimumHeight: 360
    title: "Cab Display"
    color: "#05070a"

    readonly property var aspectNames: ["GREEN", "YELLOW", "RED"]
    readonly property var aspectColors: ["#3ddc84", "#e5b93d", "#e5484d"]
    readonly property real speedoMax: 160

    Row {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 24

        Item {
            id: speedo
            width: 320
            height: parent.height

            Rectangle {
                anchors.fill: parent
                radius: 12
                color: "#0d1420"
                border.color: "#243044"
            }

            Column {
                anchors.centerIn: parent
                spacing: 6

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: train.speedKmh.toFixed(0)
                    color: train.speedKmh > train.permittedSpeedKmh + 3 ? "#e5484d" : "#f2f4f7"
                    font.pixelSize: 72
                    font.bold: true
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "km/h"
                    color: "#8a94a3"
                    font.pixelSize: 14
                }

                Rectangle {
                    width: 220
                    height: 28
                    radius: 4
                    color: "#0a0f16"
                    border.color: "#243044"

                    Rectangle {
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: parent.width * Math.min(train.speedKmh / speedoMax, 1.0)
                        color: train.speedKmh > train.permittedSpeedKmh ? "#e5484d" : "#2f81f7"
                        Behavior on width { NumberAnimation { duration: 150 } }
                    }

                    Rectangle {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: 3
                        color: "#f2f4f7"
                        x: parent.width * Math.min(train.permittedSpeedKmh / speedoMax, 1.0)
                    }
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "PERMITTED " + train.permittedSpeedKmh.toFixed(0) + " km/h"
                    color: "#8a94a3"
                    font.pixelSize: 12
                }
            }
        }

        Column {
            width: parent.width - speedo.width - 24
            height: parent.height
            spacing: 16

            Rectangle {
                width: parent.width
                height: 70
                radius: 10
                color: "#0d1420"
                border.color: "#243044"

                Row {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 20

                    Column {
                        Text { text: "MARGIN TO NEXT RESTRICTION"; color: "#8a94a3"; font.pixelSize: 10 }
                        Text {
                            text: Number.isFinite(train.distanceToRestrictionM)
                                  ? train.distanceToRestrictionM.toFixed(0) + " m -> " + train.restrictionSpeedKmh.toFixed(0) + " km/h"
                                  : "clear"
                            color: "#f2f4f7"
                            font.pixelSize: 18
                            font.bold: true
                        }
                    }

                    Rectangle { width: 1; height: parent.height; color: "#243044" }

                    Column {
                        Text { text: "NEXT STATION"; color: "#8a94a3"; font.pixelSize: 10 }
                        Text {
                            text: train.nextStationName + " · " + train.nextStationDistanceM.toFixed(0) + " m"
                            color: "#f2f4f7"
                            font.pixelSize: 18
                            font.bold: true
                        }
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 16

                Rectangle {
                    width: 140
                    height: 90
                    radius: 10
                    color: "#0d1420"
                    border.color: aspectColors[train.signalAspectIndex]

                    Column {
                        anchors.centerIn: parent
                        spacing: 8
                        Rectangle {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: 36
                            height: 36
                            radius: 18
                            color: aspectColors[train.signalAspectIndex]
                        }
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: aspectNames[train.signalAspectIndex]
                            color: "#f2f4f7"
                            font.pixelSize: 11
                            font.bold: true
                        }
                    }
                }

                Rectangle {
                    width: 140
                    height: 90
                    radius: 10
                    color: "#0d1420"
                    border.color: train.doorsOpen ? "#e5b93d" : "#243044"

                    Column {
                        anchors.centerIn: parent
                        spacing: 8
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: train.doorsOpen ? "DOORS OPEN" : "DOORS CLOSED"
                            color: train.doorsOpen ? "#e5b93d" : "#3ddc84"
                            font.pixelSize: 13
                            font.bold: true
                        }
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: train.doorsOpen ? "⛝ ⛝" : "▮▮"
                            color: "#f2f4f7"
                            font.pixelSize: 20
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: parent.height - 70 - 90 - 32
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
    }
}
