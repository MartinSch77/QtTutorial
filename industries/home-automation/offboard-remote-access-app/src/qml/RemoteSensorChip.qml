// SPDX-License-Identifier: MIT
import QtQuick

Rectangle {
    id: root

    property string sensorName: ""
    property bool triggered: false

    height: 40
    radius: 10
    color: "#1c212b"
    border.color: "#2a3140"
    border.width: 1

    Row {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 8

        DoorSensorIcon {
            width: 18
            height: 18
            anchors.verticalCenter: parent.verticalCenter
            triggered: root.triggered
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.sensorName
            color: "#e6edf3"
            font.pixelSize: 12
        }
    }
}
