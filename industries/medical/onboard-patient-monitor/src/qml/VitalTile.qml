// SPDX-License-Identifier: MIT
import QtQuick

Rectangle {
    id: root

    property string label: ""
    property string value: ""
    property color accentColor: "#39c0ff"

    radius: 10
    color: "#1c212b"
    border.color: accentColor
    border.width: 1

    Column {
        anchors.centerIn: parent
        spacing: 4

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.value
            color: root.accentColor
            font.pixelSize: 34
            font.bold: true
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.label
            color: "#9aa4b2"
            font.pixelSize: 13
        }
    }
}
