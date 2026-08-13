// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window
import IndustrialAutomation.OnboardHmiPanel

// Design reference: styled after the genre of embedded plant HMI/SCADA
// mimic screens popularised by products like Siemens WinCC and Rockwell
// FactoryTalk View (dark theme, colour-coded run/stop/fault equipment,
// a P&ID-style process line with tank/valve/conveyor/motor, an alarm
// list docked at the side) — this is a visual-language reference only.
// No trademark, logo, wordmark, or exact layout from either product is
// reproduced; every glyph on screen is drawn from scratch in QML below.
Window {
    id: window
    visible: true
    width: 1180
    height: 700
    minimumWidth: 960
    minimumHeight: 560
    title: "Plant HMI Panel — Line 1"
    color: "#12181f"

    readonly property color pipeColor: "#5a6472"
    readonly property color panelColor: "#1b2430"
    readonly property var pumpStateNames: ["IDLE", "RUNNING", "FAULT"]
    readonly property var pumpStateColors: ["#5a6472", "#3ddc84", "#e5484d"]

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        // Title strip — the kind of fixed plant/line identification banner a
        // real embedded HMI always shows so an operator standing at the
        // cabinet knows exactly which line they are looking at.
        Rectangle {
            width: parent.width
            height: 34
            radius: 6
            color: panelColor
            border.color: "#2c3644"

            Row {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                spacing: 10

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "LINE 1 — FILLING & PACKAGING"
                    color: "#f2f4f7"
                    font.pixelSize: 13
                    font.bold: true
                }
                Rectangle { width: 1; height: 18; color: "#2c3644"; anchors.verticalCenter: parent.verticalCenter }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Station: HMI-01"
                    color: "#8a94a3"
                    font.pixelSize: 11
                }
            }

            Text {
                anchors.right: parent.right
                anchors.rightMargin: 12
                anchors.verticalCenter: parent.verticalCenter
                text: Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss")
                color: "#8a94a3"
                font.pixelSize: 11

                Timer {
                    interval: 1000
                    running: true
                    repeat: true
                    onTriggered: parent.text = Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss")
                }
            }
        }

        Row {
            width: parent.width
            spacing: 12
            height: parent.height - alarmBanner.height - 34 - 24

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

                    // ---- Stage 1: pump feeding the tank (existing loop) ----
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

                    // ---- Stage 2: the tank itself, now a proper silo glyph ----
                    TankIcon {
                        id: tank
                        x: 12
                        y: mimic.height - 260
                        width: 150
                        height: 220
                        label: "TANK T-101"
                        levelPercent: processSim.tankLevel
                    }

                    // ---- Stage 3: downstream block valve, gating outflow ----
                    Rectangle {
                        id: pipeToValve
                        x: tank.x + tank.width
                        y: tank.y + tank.height - 14
                        width: 34
                        height: 6
                        color: pipeColor

                        Rectangle {
                            anchors.fill: parent
                            color: "#2f81f7"
                            opacity: processSim.valveOpen ? 0.7 : 0.15
                        }
                    }

                    ValveIcon {
                        id: valve
                        x: pipeToValve.x + pipeToValve.width
                        y: pipeToValve.y - 16
                        width: 42
                        height: 40
                        open: processSim.valveOpen

                        MouseArea {
                            anchors.fill: parent
                            onClicked: hmiApp.toggleValve()
                        }
                    }

                    Text {
                        anchors.top: valve.bottom
                        anchors.horizontalCenter: valve.horizontalCenter
                        text: "V-101\n" + (processSim.valveOpen ? "OPEN" : "CLOSED")
                        horizontalAlignment: Text.AlignHCenter
                        color: processSim.valveOpen ? "#3ddc84" : "#e5b93d"
                        font.pixelSize: 9
                        font.bold: true
                    }

                    // ---- Stage 4: conveyor moving product to the motor ----
                    Rectangle {
                        id: pipeToConveyor
                        x: valve.x + valve.width
                        y: pipeToValve.y
                        width: 20
                        height: 6
                        color: pipeColor

                        Rectangle {
                            anchors.fill: parent
                            color: "#2f81f7"
                            opacity: processSim.valveOpen ? 0.7 : 0.15
                        }
                    }

                    ConveyorIcon {
                        id: conveyor
                        x: pipeToConveyor.x + pipeToConveyor.width
                        y: pipeToValve.y - 11
                        width: mimic.width - x - 90
                        height: 28
                        speed: processSim.conveyorSpeed
                    }

                    Text {
                        anchors.top: conveyor.bottom
                        anchors.left: conveyor.left
                        anchors.topMargin: 2
                        text: "CONV C-101 · " + processSim.conveyorSpeed.toFixed(2) + " m/s"
                        color: "#8a94a3"
                        font.pixelSize: 9
                    }

                    // ---- Stage 5: motor driving the conveyor ----
                    MotorIcon {
                        id: motor
                        x: conveyor.x + conveyor.width + 8
                        y: pipeToValve.y - 24
                        width: 52
                        height: 52
                        stateIndex: processSim.motorRunning ? 1 : 0
                    }

                    Text {
                        anchors.top: motor.bottom
                        anchors.horizontalCenter: motor.horizontalCenter
                        text: "M-101\n" + (processSim.motorRunning ? "RUN" : "STOP")
                        horizontalAlignment: Text.AlignHCenter
                        color: processSim.motorRunning ? "#3ddc84" : "#5a6472"
                        font.pixelSize: 9
                        font.bold: true
                    }

                    // ---- Live values overlay ----
                    Column {
                        x: mimic.width - 180
                        y: mimic.height - 220
                        spacing: 10

                        Rectangle {
                            width: 180
                            height: 128
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
                                Text { text: "CONVEYOR SPEED"; color: "#8a94a3"; font.pixelSize: 10 }
                                Text {
                                    text: processSim.conveyorSpeed.toFixed(2) + " m/s"
                                    color: "#f2f4f7"
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

                AlarmAckPanel {
                    anchors.fill: parent
                    anchors.margins: 16
                    alarmModel: alarmLog
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

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 12
                visible: alarmLog.unacknowledgedCount > 0
                text: alarmLog.unacknowledgedCount + " unacknowledged"
                color: "#e5b93d"
                font.pixelSize: 11
                font.bold: true
            }
        }
    }
}
