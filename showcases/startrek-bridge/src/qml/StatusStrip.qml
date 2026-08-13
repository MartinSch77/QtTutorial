// SPDX-License-Identifier: MIT
import QtQuick

// LCARS-styled top status strip, always visible regardless of which
// station is selected: ship name placeholder, a simulated stardate-style
// readout, and the shared ship-wide alert-level condition indicator. Red
// alert pulses so it reads as an emergency state at a glance.
Rectangle {
    id: root
    property var bridge
    readonly property var alertColors: ["#2fa84f", "#e0a300", "#c0392b"]
    readonly property var alertLabels: [qsTr("CONDITION GREEN"), qsTr("CONDITION YELLOW"), qsTr("RED ALERT")]
    readonly property color currentAlertColor: bridge ? alertColors[bridge.alertLevelIndex] : alertColors[0]

    color: "#0c0910"

    Row {
        anchors.fill: parent
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        spacing: 24

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.bridge ? root.bridge.shipName : ""
            color: "#f2e9ff"
            font.pixelSize: 16
            font.bold: true
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.bridge ? qsTr("STARDATE %1").arg(root.bridge.stardate) : ""
            color: "#c6a0ff"
            font.pixelSize: 14
        }
    }

    Row {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.rightMargin: 16
        spacing: 10

        Row {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 4

            Repeater {
                model: root.alertColors
                Rectangle {
                    width: 26
                    height: 26
                    radius: 13
                    color: root.bridge && root.bridge.alertLevelIndex === index ? modelData : "#221c2b"
                    border.width: 1
                    border.color: modelData

                    MouseArea {
                        anchors.fill: parent
                        onClicked: if (root.bridge) root.bridge.setAlertLevel(index)
                    }
                }
            }
        }

        Rectangle {
            id: alertDot
            width: 16
            height: 16
            radius: 8
            color: root.currentAlertColor
            anchors.verticalCenter: parent.verticalCenter

            SequentialAnimation on opacity {
                running: root.bridge && root.bridge.alertLevelIndex === 2
                loops: Animation.Infinite
                NumberAnimation { from: 1.0; to: 0.25; duration: 400 }
                NumberAnimation { from: 0.25; to: 1.0; duration: 400 }
            }
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.bridge ? root.alertLabels[root.bridge.alertLevelIndex] : ""
            color: root.currentAlertColor
            font.pixelSize: 15
            font.bold: true
        }
    }
}
