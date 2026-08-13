// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window

Window {
    id: window
    visible: true
    width: 960
    height: 600
    minimumWidth: 800
    minimumHeight: 480
    title: "Plant HMI Panel"
    color: "#12181f"

    readonly property color pipeColor: "#5a6472"
    readonly property color panelColor: "#1b2430"
    readonly property var pumpStateNames: ["IDLE", "RUNNING", "FAULT"]
    readonly property var pumpStateColors: ["#5a6472", "#3ddc84", "#e5484d"]

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Row {
            width: parent.width
            spacing: 12
            height: parent.height - alarmBanner.height - 12

            Rectangle {
                id: mimicPanel
                width: parent.width * 0.68
                height: parent.height
                radius: 8
                color: panelColor
                border.color: "#2c3644"

                Item {
                    id: mimic
                    anchors.fill: parent
                    anchors.margins: 24

                    Rectangle {
                        id: pipeIn
                        x: 40
                        y: 40
                        width: 6
                        height: tank.y - y
                        color: pipeColor

                        Rectangle {
                            visible: pump.stateIndex === 1
                            anchors.fill: parent
                            color: "#3ddc84"
                            opacity: flowPulse.value

                            SequentialAnimation {
                                id: flowAnim
                                running: pump.stateIndex === 1
                                loops: Animation.Infinite
                                PropertyAnimation { target: flowPulse; property: "value"; from: 0.2; to: 1.0; duration: 400 }
                                PropertyAnimation { target: flowPulse; property: "value"; from: 1.0; to: 0.2; duration: 400 }
                            }
                            QtObject { id: flowPulse; property real value: 0.2 }
                        }
                    }

                    Rectangle {
                        id: pumpBox
                        x: pipeIn.x - 24
                        y: pipeIn.y - 44
                        width: 60
                        height: 36
                        radius: 6
                        color: pumpStateColors[pump.stateIndex]
                        border.color: "#0d1116"

                        Text {
                            anchors.centerIn: parent
                            text: "PUMP"
                            color: "#0d1116"
                            font.bold: true
                            font.pixelSize: 11
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: hmiApp.togglePump()
                        }
                    }

                    Text {
                        anchors.top: pumpBox.bottom
                        anchors.horizontalCenter: pumpBox.horizontalCenter
                        anchors.topMargin: 4
                        text: pumpStateNames[pump.stateIndex]
                        color: pumpStateColors[pump.stateIndex]
                        font.pixelSize: 10
                        font.bold: true
                    }

                    Rectangle {
                        id: tank
                        x: 20
                        y: mimic.height - 260
                        width: 160
                        height: 220
                        radius: 4
                        color: "#0d1116"
                        border.color: pipeColor
                        border.width: 2

                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            height: parent.height * Math.min(Math.max(processSim.tankLevel, 0), 100) / 100
                            color: processSim.tankLevel > 90 ? "#e5484d" : (processSim.tankLevel < 10 ? "#e5b93d" : "#2f81f7")

                            Behavior on height { NumberAnimation { duration: 200 } }
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 4
                            text: "TANK T-101"
                            color: "#c8d0da"
                            font.pixelSize: 11
                        }

                        Text {
                            anchors.centerIn: parent
                            text: processSim.tankLevel.toFixed(1) + " %"
                            color: "#f2f4f7"
                            font.pixelSize: 16
                            font.bold: true
                        }
                    }

                    Rectangle {
                        x: tank.x + tank.width
                        y: tank.y + tank.height - 12
                        width: mimic.width - tank.x - tank.width - 40
                        height: 6
                        color: pipeColor

                        Rectangle {
                            anchors.fill: parent
                            color: "#2f81f7"
                            opacity: 0.6
                        }
                    }

                    Column {
                        x: mimic.width - 180
                        y: mimic.height - 220
                        spacing: 10

                        Rectangle {
                            width: 180
                            height: 90
                            radius: 6
                            color: "#0d1116"
                            border.color: pipeColor

                            Column {
                                anchors.centerIn: parent
                                spacing: 4
                                Text { text: "FLOW RATE"; color: "#8a94a3"; font.pixelSize: 10 }
                                Text {
                                    text: processSim.flowRate.toFixed(2) + " m³/min"
                                    color: "#f2f4f7"
                                    font.pixelSize: 18
                                    font.bold: true
                                }
                                Text { text: "PRESSURE"; color: "#8a94a3"; font.pixelSize: 10 }
                                Text {
                                    text: processSim.pressure.toFixed(2) + " bar"
                                    color: processSim.pressure > 7.5 ? "#e5484d" : "#f2f4f7"
                                    font.pixelSize: 18
                                    font.bold: true
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width * 0.30
                height: parent.height
                radius: 8
                color: panelColor
                border.color: "#2c3644"

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10

                    Text { text: "Alarm history"; color: "#f2f4f7"; font.pixelSize: 14; font.bold: true }

                    ListView {
                        width: parent.width
                        height: parent.height - 30
                        clip: true
                        model: alarmLog
                        spacing: 6
                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 44
                            radius: 4
                            color: severity === 2 ? "#3a1d20" : (severity === 1 ? "#3a341a" : "#1d2733")
                            border.color: severity === 2 ? "#e5484d" : (severity === 1 ? "#e5b93d" : "#2c3644")

                            Column {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.leftMargin: 8
                                Text { text: message; color: "#f2f4f7"; font.pixelSize: 11 }
                                Text {
                                    text: Qt.formatTime(timestamp, "hh:mm:ss")
                                    color: "#8a94a3"
                                    font.pixelSize: 9
                                }
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: alarmBanner
            width: parent.width
            height: 40
            radius: 6
            color: alarmLog.latestSeverity === 2 ? "#3a1d20" : "#1d2733"
            border.color: "#e5484d"
            visible: alarmLog.count > 0

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 12
                text: alarmLog.count > 0 ? "⚠ " + alarmLog.latestMessage : ""
                color: "#f2f4f7"
                font.pixelSize: 13
                font.bold: true
            }
        }
    }
}
