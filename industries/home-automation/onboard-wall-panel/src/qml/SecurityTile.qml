// SPDX-License-Identifier: MIT
import QtQuick

Rectangle {
    id: root

    property string sensorName: ""
    property bool triggered: false

    radius: 14
    color: "#1c212b"
    border.color: "#2a3140"
    border.width: 1

    Row {
        anchors.centerIn: parent
        spacing: 10

        DoorSensorIcon {
            width: 26
            height: 26
            anchors.verticalCenter: parent.verticalCenter
            triggered: root.triggered
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.sensorName
            color: "#e6edf3"
            font.pixelSize: 15
        }
    }
}
