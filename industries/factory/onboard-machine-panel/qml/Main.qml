// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window
import QtQuick.Controls

Window {
    id: window
    visible: true
    width: 1024
    height: 640
    minimumWidth: 900
    minimumHeight: 560
    title: "Onboard Machine Panel"
    color: "#0b0d10"

    readonly property var stateNames: ["IDLE", "RUNNING", "PAUSED", "WARNING", "FAULT"]
    readonly property var stateColors: ["#8a94a3", "#3ddcc8", "#5a6472", "#e5b93d", "#e5484d"]
    readonly property color panelColor: "#151a21"
    readonly property color panelBorder: "#262f3b"
    readonly property color glassOverlay: "#22ffffff"
    readonly property color accentCyan: "#3ddcc8"
    readonly property color accentAmber: "#e5b93d"
    readonly property color accentRed: "#e5484d"

    readonly property int currentState: machineState.stateIndex
    readonly property color currentColor: stateColors[currentState]

    Rectangle {
        anchors.fill: parent
        color: "transparent"

        Column {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 14

            // Status header
            Rectangle {
                width: parent.width
                height: 64
                radius: 10
                color: panelColor
                border.color: panelBorder

                Behavior on color { ColorAnimation { duration: 250 } }

                Row {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 16

                    Rectangle {
                        id: statusDot
                        width: 22
                        height: 22
                        radius: 11
                        anchors.verticalCenter: parent.verticalCenter
                        color: currentColor
                        Behavior on color { ColorAnimation { duration: 250 } }

                        SequentialAnimation on opacity {
                            running: currentState === 4 || currentState === 3
                            loops: Animation.Infinite
                            NumberAnimation { from: 1.0; to: 0.35; duration: 500 }
                            NumberAnimation { from: 0.35; to: 1.0; duration: 500 }
                        }
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: "MACHINE STATE: " + stateNames[currentState]
                        color: "#f2f4f7"
                        font.pixelSize: 20
                        font.bold: true
                        font.letterSpacing: 1
                    }

                    Item { width: 1; height: 1 }
                }
            }

            Row {
                width: parent.width
                height: parent.height - 64 - 130 - 28
                spacing: 14

                // Left: activity + gauges
                Rectangle {
                    width: parent.width * 0.62
                    height: parent.height
                    radius: 10
                    color: panelColor
                    border.color: panelBorder

                    Column {
                        anchors.fill: parent
                        anchors.margins: 18
                        spacing: 16

                        Text {
                            text: "PRODUCTION CYCLE"
                            color: "#8a94a3"
                            font.pixelSize: 11
                            font.letterSpacing: 1
                        }

                        Row {
                            spacing: 24
                            height: 120

                            // Rotating activity icon
                            Item {
                                width: 110
                                height: 110

                                Rectangle {
                                    anchors.fill: parent
                                    radius: width / 2
                                    color: "#0d1116"
                                    border.color: panelBorder
                                    border.width: 2
                                }

                                Rectangle {
                                    id: spinner
                                    anchors.centerIn: parent
                                    width: 70
                                    height: 10
                                    radius: 5
                                    color: currentColor
                                    Behavior on color { ColorAnimation { duration: 250 } }

                                    RotationAnimation {
                                        target: spinner
                                        running: currentState === 1
                                        from: 0
                                        to: 360
                                        duration: 1200
                                        loops: Animation.Infinite
                                    }
                                }
                            }

                            Column {
                                spacing: 10
                                anchors.verticalCenter: parent.verticalCenter

                                Text {
                                    text: "CYCLE #" + machineSim.cycleCount
                                    color: "#f2f4f7"
                                    font.pixelSize: 22
                                    font.bold: true
                                }

                                Rectangle {
                                    width: 220
                                    height: 14
                                    radius: 7
                                    color: "#0d1116"
                                    border.color: panelBorder

                                    Rectangle {
                                        height: parent.height
                                        radius: 7
                                        width: parent.width * Math.min(Math.max(machineSim.cycleProgress, 0), 1)
                                        color: currentColor
                                        Behavior on width { NumberAnimation { duration: 200 } }
                                        Behavior on color { ColorAnimation { duration: 250 } }
                                    }
                                }
                            }
                        }

                        Text {
                            text: "SENSOR FEED"
                            color: "#8a94a3"
                            font.pixelSize: 11
                            font.letterSpacing: 1
                        }

                        Grid {
                            columns: 2
                            columnSpacing: 14
                            rowSpacing: 14
                            width: parent.width

                            Repeater {
                                model: [
                                    { label: "SPEED", value: machineSim.speed.toFixed(0) + " RPM", warn: false, fault: false },
                                    { label: "TEMPERATURE", value: machineSim.temperature.toFixed(1) + " °C",
                                      warn: machineSim.temperature >= 80.0, fault: machineSim.temperature >= 95.0 },
                                    { label: "VIBRATION", value: machineSim.vibration.toFixed(2) + " mm/s",
                                      warn: machineSim.vibration >= 5.0, fault: machineSim.vibration >= 7.0 },
                                    { label: "TARGET SPEED", value: machineSim.targetSpeed.toFixed(0) + " RPM", warn: false, fault: false }
                                ]

                                Rectangle {
                                    width: (parent.width - 14) / 2
                                    height: 64
                                    radius: 8
                                    color: "#0d1116"
                                    border.color: modelData.fault ? accentRed : (modelData.warn ? accentAmber : panelBorder)
                                    border.width: (modelData.fault || modelData.warn) ? 2 : 1
                                    Behavior on border.color { ColorAnimation { duration: 250 } }

                                    Column {
                                        anchors.centerIn: parent
                                        spacing: 3
                                        Text { text: modelData.label; color: "#8a94a3"; font.pixelSize: 10; font.letterSpacing: 1 }
                                        Text {
                                            text: modelData.value
                                            color: modelData.fault ? accentRed : (modelData.warn ? accentAmber : "#f2f4f7")
                                            font.pixelSize: 17
                                            font.bold: true
                                            Behavior on color { ColorAnimation { duration: 250 } }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Right: event log
                Rectangle {
                    width: parent.width * 0.36
                    height: parent.height
                    radius: 10
                    color: panelColor
                    border.color: panelBorder

                    Column {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 10

                        Text { text: "EVENT LOG"; color: "#f2f4f7"; font.pixelSize: 13; font.bold: true; font.letterSpacing: 1 }

                        ListView {
                            width: parent.width
                            height: parent.height - 26
                            clip: true
                            model: eventLog
                            spacing: 6
                            verticalLayoutDirection: ListView.TopToBottom
                            delegate: Rectangle {
                                width: ListView.view.width
                                height: 46
                                radius: 6
                                color: severity === 2 ? "#3a1d20" : (severity === 1 ? "#3a341a" : "#171d26")
                                border.color: severity === 2 ? accentRed : (severity === 1 ? accentAmber : panelBorder)

                                Column {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
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

            // Control panel
            Rectangle {
                width: parent.width
                height: 130
                radius: 10
                color: panelColor
                border.color: panelBorder

                Row {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 18

                    Row {
                        spacing: 10
                        anchors.verticalCenter: parent.verticalCenter

                        Button {
                            text: "START"
                            enabled: currentState === 0 || currentState === 2
                            onClicked: panelApp.requestStart()
                        }
                        Button {
                            text: "PAUSE"
                            enabled: currentState === 1
                            onClicked: panelApp.requestPause()
                        }
                        Button {
                            text: "STOP"
                            enabled: currentState === 1 || currentState === 2 || currentState === 3
                            onClicked: panelApp.requestStop()
                        }
                        Button {
                            text: "ACKNOWLEDGE ALARM"
                            enabled: currentState === 4
                            highlighted: currentState === 4
                            onClicked: panelApp.requestAcknowledge()
                        }
                    }

                    Item { width: 1; height: 1 }

                    Column {
                        spacing: 6
                        anchors.verticalCenter: parent.verticalCenter

                        Text { text: "SETPOINT (RPM)"; color: "#8a94a3"; font.pixelSize: 10; font.letterSpacing: 1 }

                        Row {
                            spacing: 8

                            TextField {
                                id: setpointField
                                width: 140
                                placeholderText: "0-3000"
                                validator: DoubleValidator { bottom: 0; top: 3000 }
                                selectByMouse: true
                            }

                            Button {
                                text: "APPLY"
                                onClicked: {
                                    const value = Number(setpointField.text)
                                    if (setpointField.text.length > 0 && !isNaN(value)) {
                                        panelApp.requestSetpoint(value)
                                    }
                                }
                            }
                        }

                        Text {
                            text: "Current: " + machineSim.targetSpeed.toFixed(0) + " RPM"
                            color: "#5a6472"
                            font.pixelSize: 10
                        }
                    }
                }
            }
        }
    }
}
