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

        Rectangle {
            width: 14
            height: 14
            radius: 7
            anchors.verticalCenter: parent.verticalCenter
            color: root.triggered ? "#d4a12b" : "#3ddc6f"

            Behavior on color { ColorAnimation { duration: 200 } }
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.sensorName
            color: "#e6edf3"
            font.pixelSize: 15
        }
    }
}
