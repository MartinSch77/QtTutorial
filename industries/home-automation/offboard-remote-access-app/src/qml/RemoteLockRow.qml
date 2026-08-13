// SPDX-License-Identifier: MIT
import QtQuick

Rectangle {
    id: root

    property string lockName: ""
    property bool locked: true

    signal toggled(bool locked)

    height: 52
    radius: 12
    color: "#1c212b"
    border.color: locked ? "#3ddc6f" : "#c0392b"
    border.width: 1

    Behavior on border.color { ColorAnimation { duration: 200 } }

    Row {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        LockIcon {
            width: 20
            height: 20
            anchors.verticalCenter: parent.verticalCenter
            locked: root.locked
        }
        Text {
            width: parent.width - 88
            anchors.verticalCenter: parent.verticalCenter
            text: root.lockName
            color: "#e6edf3"
            font.pixelSize: 14
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.locked ? qsTr("Locked") : qsTr("Unlocked")
            color: root.locked ? "#3ddc6f" : "#c0392b"
            font.pixelSize: 13
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.toggled(!root.locked)
    }
}
