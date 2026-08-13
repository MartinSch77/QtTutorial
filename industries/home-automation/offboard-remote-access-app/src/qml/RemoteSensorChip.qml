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

        Rectangle {
            width: 10
            height: 10
            radius: 5
            anchors.verticalCenter: parent.verticalCenter
            color: root.triggered ? "#d4a12b" : "#3ddc6f"
            Behavior on color { ColorAnimation { duration: 200 } }
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.sensorName
            color: "#e6edf3"
            font.pixelSize: 12
        }
    }
}
