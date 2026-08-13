// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window
import QtTutorial.HomeAutomation.Offboard

// Phone-shaped remote-access app: the same device categories as the wall
// panel, but in a compact, list-based layout suitable for a small screen,
// talking to the home system over Qt6::Network rather than controlling it
// directly. In this self-contained demo RemoteHomeController also hosts the
// simulated home locally; in a real deployment it would just be the client
// half, pointed at the home's actual address.
Window {
    id: window
    width: 380
    height: 720
    visible: true
    color: "#0d1117"
    title: qsTr("QtTutorial - Home Remote")

    RemoteHomeController { id: controller }

    Column {
        anchors.fill: parent

        Rectangle {
            width: parent.width
            height: 56
            color: "#141a24"

            Row {
                anchors.fill: parent
                anchors.margins: 14
                spacing: 8

                Rectangle {
                    width: 10
                    height: 10
                    radius: 5
                    anchors.verticalCenter: parent.verticalCenter
                    color: controller.connected ? "#3ddc6f" : "#c0392b"
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: controller.connected ? qsTr("Connected to home") : qsTr("Connecting...")
                    color: "#e6edf3"
                    font.pixelSize: 14
                    font.bold: true
                }
            }
        }

        Flickable {
            width: parent.width
            height: window.height - 56
            contentWidth: width
            contentHeight: content.height
            clip: true

            Column {
                id: content
                width: parent.width
                spacing: 18
                topPadding: 16
                bottomPadding: 24
                leftPadding: 16
                rightPadding: 16

                Text { text: qsTr("ROOMS"); color: "#9aa4b2"; font.pixelSize: 12; font.bold: true }
                ListView {
                    width: parent.width - 32
                    height: contentHeight
                    interactive: false
                    spacing: 10
                    model: controller.rooms
                    delegate: RemoteRoomRow {
                        width: ListView.view.width
                        roomName: modelData.name
                        lightOn: modelData.lightOn
                        brightness: modelData.brightness
                        blindPosition: modelData.blindPosition
                        onLightToggled: (on) => controller.setLightOn(modelData.name, on)
                        onBrightnessMoved: (value) => controller.setBrightness(modelData.name, Math.round(value))
                        onBlindMoved: (value) => controller.setBlindPosition(modelData.name, Math.round(value))
                    }
                }

                Text { text: qsTr("LOCKS"); color: "#9aa4b2"; font.pixelSize: 12; font.bold: true }
                ListView {
                    width: parent.width - 32
                    height: contentHeight
                    interactive: false
                    spacing: 8
                    model: controller.locks
                    delegate: RemoteLockRow {
                        width: ListView.view.width
                        lockName: modelData.name
                        locked: modelData.locked
                        onToggled: (locked) => controller.setLockLocked(modelData.name, locked)
                    }
                }

                Text { text: qsTr("CLIMATE"); color: "#9aa4b2"; font.pixelSize: 12; font.bold: true }
                Rectangle {
                    width: parent.width - 32
                    height: 130
                    radius: 12
                    color: "#1c212b"
                    border.color: "#2a3140"
                    border.width: 1

                    Column {
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 10

                        Row {
                            width: parent.width
                            Text {
                                width: parent.width - 100
                                text: qsTr("%1°C (target %2°C)").arg(controller.thermostatCurrent.toFixed(1)).arg(controller.thermostatTarget.toFixed(1))
                                color: "#e6edf3"
                                font.pixelSize: 15
                                font.bold: true
                            }
                            Repeater {
                                model: [
                                    {label: qsTr("Off"), mode: 0},
                                    {label: qsTr("Heat"), mode: 1},
                                    {label: qsTr("Cool"), mode: 2},
                                ]
                                Rectangle {
                                    width: 30
                                    height: 26
                                    radius: 6
                                    color: controller.thermostatMode === modelData.mode ? "#39c0ff" : "#2a3140"
                                    Text {
                                        anchors.centerIn: parent
                                        text: modelData.label.charAt(0)
                                        color: controller.thermostatMode === modelData.mode ? "#0d1117" : "#e6edf3"
                                        font.pixelSize: 11
                                        font.bold: true
                                    }
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: controller.setThermostatMode(modelData.mode)
                                    }
                                }
                            }
                        }

                        SliderControl {
                            width: parent.width
                            from: 10
                            to: 30
                            value: controller.thermostatTarget
                            accentColor: "#ff9f43"
                            onMoved: (value) => controller.setThermostatTarget(Math.round(value * 2) / 2)
                        }
                    }
                }

                Text { text: qsTr("SECURITY"); color: "#9aa4b2"; font.pixelSize: 12; font.bold: true }
                Row {
                    width: parent.width - 32
                    Text {
                        width: parent.width - 60
                        anchors.verticalCenter: parent.verticalCenter
                        text: controller.armed ? qsTr("Armed") : qsTr("Disarmed")
                        color: controller.armed ? "#3ddc6f" : "#9aa4b2"
                        font.pixelSize: 14
                        font.bold: true
                    }
                    ToggleSwitch {
                        checked: controller.armed
                        onColor: "#3ddc6f"
                        onToggled: (checked) => controller.setArmed(checked)
                    }
                }
                ListView {
                    width: parent.width - 32
                    height: contentHeight
                    interactive: false
                    spacing: 6
                    model: controller.sensors
                    delegate: RemoteSensorChip {
                        width: ListView.view.width
                        sensorName: modelData.name
                        triggered: modelData.triggered
                    }
                }

                Text { text: qsTr("ACTIVITY"); color: "#9aa4b2"; font.pixelSize: 12; font.bold: true }
                ListView {
                    width: parent.width - 32
                    height: contentHeight
                    interactive: false
                    spacing: 4
                    model: controller.activityLog
                    delegate: Text {
                        width: ListView.view.width
                        text: modelData
                        color: "#9aa4b2"
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }
}
