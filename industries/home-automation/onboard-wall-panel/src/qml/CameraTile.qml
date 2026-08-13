// SPDX-License-Identifier: MIT
import QtQuick

// A camera-feed placeholder tile, the kind a Google Home/Apple
// Home/Control4-style security dashboard shows per camera. There is no real
// video (this example has no network/camera hardware dependency per
// REQ-IND-04), just a labelled placeholder with a live/idle indicator and a
// slow animated scanline standing in for "a feed is active".
Rectangle {
    id: root

    property string cameraName: ""
    property bool live: true

    radius: 14
    color: "#0d1117"
    border.color: "#2a3140"
    border.width: 1
    clip: true

    Rectangle {
        id: scanline
        width: parent.width
        height: 2
        color: "#39c0ff"
        opacity: root.live ? 0.5 : 0.0
        y: 0

        SequentialAnimation on y {
            loops: Animation.Infinite
            running: root.live
            NumberAnimation { from: 0; to: root.height; duration: 2600; easing.type: Easing.InOutSine }
            NumberAnimation { from: root.height; to: 0; duration: 2600; easing.type: Easing.InOutSine }
        }
    }

    Column {
        anchors.centerIn: parent
        spacing: 8

        CameraIcon {
            width: 30
            height: 30
            anchors.horizontalCenter: parent.horizontalCenter
            active: root.live
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.cameraName
            color: "#e6edf3"
            font.pixelSize: 13
            font.bold: true
        }
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 4

            Rectangle {
                width: 8
                height: 8
                radius: 4
                anchors.verticalCenter: parent.verticalCenter
                color: root.live ? "#c0392b" : "#4a5568"
            }
            Text {
                text: root.live ? qsTr("LIVE") : qsTr("OFFLINE")
                color: "#9aa4b2"
                font.pixelSize: 11
            }
        }
    }
}
