// SPDX-License-Identifier: MIT
import QtQuick

Rectangle {
    id: root

    property string lockName: ""
    property bool locked: true

    signal toggled(bool locked)

    radius: 14
    color: "#1c212b"
    border.color: locked ? "#3ddc6f" : "#c0392b"
    border.width: 1

    Behavior on border.color { ColorAnimation { duration: 200 } }

    Column {
        anchors.centerIn: parent
        spacing: 6

        LockIcon {
            width: 28
            height: 28
            anchors.horizontalCenter: parent.horizontalCenter
            locked: root.locked
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.lockName
            color: "#e6edf3"
            font.pixelSize: 16
            font.bold: true
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.locked ? qsTr("Locked") : qsTr("Unlocked")
            color: root.locked ? "#3ddc6f" : "#c0392b"
            font.pixelSize: 14
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.toggled(!root.locked)
    }
}
