// SPDX-License-Identifier: MIT
import QtQuick

Rectangle {
    id: root

    property string roomName: ""
    property int position: 0

    signal positionMoved(real value)

    radius: 14
    color: "#1c212b"
    border.color: "#2a3140"
    border.width: 1

    Row {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 18

        Rectangle {
            id: window
            width: 40
            height: parent.height
            radius: 4
            color: "#0d1117"
            border.color: "#4a5568"
            border.width: 1

            Rectangle {
                width: parent.width - 4
                anchors.top: parent.top
                anchors.topMargin: 2
                anchors.horizontalCenter: parent.horizontalCenter
                height: (parent.height - 4) * (root.position / 100)
                color: "#39c0ff"
                opacity: 0.6
                radius: 2

                Behavior on height { NumberAnimation { duration: 150 } }
            }
        }

        Column {
            width: parent.width - window.width - parent.spacing
            spacing: 12
            anchors.verticalCenter: parent.verticalCenter

            Text {
                text: root.roomName
                color: "#e6edf3"
                font.pixelSize: 18
                font.bold: true
            }
            Text {
                text: qsTr("Shade open %1%").arg(root.position)
                color: "#9aa4b2"
                font.pixelSize: 13
            }
            SliderControl {
                width: parent.width
                from: 0
                to: 100
                value: root.position
                accentColor: "#39c0ff"
                onMoved: root.positionMoved(value)
            }
        }
    }
}
