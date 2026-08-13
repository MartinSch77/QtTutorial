// SPDX-License-Identifier: MIT
import QtQuick

Rectangle {
    id: root

    property string label: ""
    property string value: ""
    property color accentColor: "#39c0ff"
    // One of ClinicalIcon's "kind" values, or "" for no icon.
    property string iconKind: ""

    radius: 10
    color: "#1c212b"
    border.color: accentColor
    border.width: 1

    Row {
        anchors.centerIn: parent
        spacing: 10

        ClinicalIcon {
            visible: root.iconKind !== ""
            anchors.verticalCenter: parent.verticalCenter
            width: 26
            height: 26
            kind: root.iconKind
            color: root.accentColor
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
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
}
