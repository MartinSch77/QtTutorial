// SPDX-License-Identifier: MIT
import QtQuick

Rectangle {
    id: root

    property string label: ""
    property bool active: false

    signal clicked()

    width: parent ? parent.width : 160
    height: 56
    color: active ? "#1c2436" : "transparent"
    radius: 8

    Rectangle {
        visible: root.active
        width: 4
        height: parent.height - 16
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        color: "#39c0ff"
        radius: 2
    }

    Text {
        anchors.centerIn: parent
        text: root.label
        color: root.active ? "#e6edf3" : "#9aa4b2"
        font.pixelSize: 15
        font.bold: root.active
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
