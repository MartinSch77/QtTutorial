// SPDX-License-Identifier: MIT
import QtQuick

// One tappable scene card: an icon, the scene's name, and a one-line summary
// of what it changes across every device category at once - the
// "room-by-room dashboard, but for routines" card style this genre of app is
// known for.
Rectangle {
    id: root

    property string sceneName: ""
    property string sceneDescription: ""
    property int sceneId: 0
    property bool active: false

    signal activated()

    radius: 16
    color: active ? "#1c2436" : "#1c212b"
    border.color: active ? "#39c0ff" : "#2a3140"
    border.width: active ? 2 : 1

    Behavior on border.color { ColorAnimation { duration: 200 } }

    Column {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 10

        Item {
            width: 36
            height: 36

            HouseIcon {
                visible: root.sceneId === 0
                anchors.fill: parent
                strokeColor: "#39c0ff"
            }
            LockIcon {
                visible: root.sceneId === 1
                anchors.fill: parent
                locked: true
            }
            SunMoonIcon {
                visible: root.sceneId === 2
                anchors.fill: parent
                sun: false
            }
            SunMoonIcon {
                visible: root.sceneId === 3
                anchors.fill: parent
                sun: true
            }
        }

        Text {
            text: root.sceneName
            color: "#e6edf3"
            font.pixelSize: 18
            font.bold: true
        }
        Text {
            width: parent.width
            text: root.sceneDescription
            color: "#9aa4b2"
            font.pixelSize: 12
            wrapMode: Text.WordWrap
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.activated()
    }
}
