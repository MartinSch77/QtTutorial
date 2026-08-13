// SPDX-License-Identifier: MIT
import QtQuick

// One headline in the scrolling band. A "breaking" headline gets a distinct,
// continuously pulsing accent treatment so it reads as urgent even while
// scrolling past at speed.
Row {
    id: root
    required property string headlineText
    required property string headlineCategory
    required property bool breaking

    spacing: 10
    height: 32

    Rectangle {
        visible: root.breaking
        anchors.verticalCenter: parent.verticalCenter
        width: breakingLabel.implicitWidth + 12
        height: 22
        radius: 4
        color: "#e5484d"

        Text {
            id: breakingLabel
            anchors.centerIn: parent
            text: qsTr("BREAKING")
            color: "white"
            font.pixelSize: 11
            font.bold: true
        }

        SequentialAnimation on opacity {
            running: root.breaking
            loops: Animation.Infinite
            NumberAnimation { from: 1.0; to: 0.35; duration: 500 }
            NumberAnimation { from: 0.35; to: 1.0; duration: 500 }
        }
    }

    Text {
        anchors.verticalCenter: parent.verticalCenter
        text: "[" + root.headlineCategory + "] " + root.headlineText
        color: root.breaking ? "#ff8a80" : "white"
        font.pixelSize: 16
        font.bold: root.breaking
    }
}
