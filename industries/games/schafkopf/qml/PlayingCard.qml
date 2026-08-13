// SPDX-License-Identifier: MIT
import QtQuick

Item {
    id: root
    property string suit: "Herz"
    property string rank: "Ass"
    property bool legal: true
    property bool dimmed: false
    width: 76
    height: 108

    readonly property var suitColors: ({
        "Eichel": "#8a6d3b",
        "Gras": "#3f8f4f",
        "Herz": "#c0392b",
        "Schellen": "#c9a227"
    })
    readonly property string suitColor: suitColors[suit] ?? "#333333"
    readonly property var rankAbbrev: ({
        "Ass": "A", "Koenig": "K", "Ober": "O", "Unter": "U",
        "Zehn": "10", "Neun": "9", "Acht": "8", "Sieben": "7"
    })
    readonly property string rankText: rankAbbrev[rank] ?? rank

    scale: dimmed ? 0.94 : 1.0
    opacity: dimmed ? 0.45 : 1.0

    Behavior on scale { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }
    Behavior on opacity { NumberAnimation { duration: 150 } }
    Behavior on y { NumberAnimation { duration: 260; easing.type: Easing.OutCubic } }
    Behavior on x { NumberAnimation { duration: 260; easing.type: Easing.OutCubic } }

    Rectangle {
        id: face
        anchors.fill: parent
        radius: 8
        color: "#fbf6ec"
        border.width: legal ? 3 : 1
        border.color: legal ? "#3ddc84" : "#9c9484"

        Column {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 6
            spacing: 0
            Text { text: root.rankText; color: root.suitColor; font.pixelSize: 18; font.bold: true }
            Text { text: root.suit.charAt(0); color: root.suitColor; font.pixelSize: 12 }
        }

        Rectangle {
            anchors.centerIn: parent
            width: 30
            height: 30
            radius: root.suit === "Schellen" ? 15 : 6
            rotation: root.suit === "Gras" ? 45 : 0
            color: root.suitColor
            opacity: 0.85
        }

        Column {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 6
            spacing: 0
            rotation: 180
            Text { text: root.rankText; color: root.suitColor; font.pixelSize: 18; font.bold: true }
            Text { text: root.suit.charAt(0); color: root.suitColor; font.pixelSize: 12 }
        }
    }
}
