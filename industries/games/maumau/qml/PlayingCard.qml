// SPDX-License-Identifier: MIT
import QtQuick

Item {
    id: root
    property string rank: "Ace"
    property string suit: "Clubs"
    property bool faceUp: true
    property bool legal: true
    property bool highlighted: false
    width: 84
    height: 118

    readonly property bool isRed: suit === "Hearts" || suit === "Diamonds"
    readonly property string pip: suit === "Clubs" ? "♣"
        : suit === "Spades" ? "♠"
        : suit === "Hearts" ? "♥" : "♦"
    readonly property string shortRank: rank === "Ten" ? "10"
        : rank === "Nine" ? "9" : rank === "Seven" ? "7" : rank === "Eight" ? "8"
        : rank.charAt(0)

    opacity: legal ? 1.0 : 0.4
    Behavior on opacity { NumberAnimation { duration: 150 } }
    Behavior on scale { NumberAnimation { duration: 150; easing.type: Easing.OutBack } }
    scale: highlighted ? 1.08 : 1.0

    Rectangle {
        id: face
        anchors.fill: parent
        radius: 10
        color: root.faceUp ? "#fbf8f2" : "#1b3a63"
        border.width: root.highlighted ? 3 : 1
        border.color: root.highlighted ? "#e0a94a" : "#8a8272"

        Column {
            visible: root.faceUp
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 6
            spacing: -2
            Text {
                text: root.shortRank
                color: root.isRed ? "#c0392b" : "#1c232c"
                font.pixelSize: 18
                font.bold: true
            }
            Text {
                text: root.pip
                color: root.isRed ? "#c0392b" : "#1c232c"
                font.pixelSize: 16
            }
        }

        Text {
            visible: root.faceUp
            anchors.centerIn: parent
            text: root.pip
            color: root.isRed ? "#c0392b" : "#1c232c"
            font.pixelSize: 40
        }

        Column {
            visible: root.faceUp
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 6
            spacing: -2
            rotation: 180
            Text {
                text: root.shortRank
                color: root.isRed ? "#c0392b" : "#1c232c"
                font.pixelSize: 18
                font.bold: true
            }
            Text {
                text: root.pip
                color: root.isRed ? "#c0392b" : "#1c232c"
                font.pixelSize: 16
            }
        }

        Grid {
            visible: !root.faceUp
            anchors.centerIn: parent
            columns: 3
            rows: 4
            spacing: 4
            Repeater {
                model: 12
                Rectangle {
                    width: 8
                    height: 8
                    radius: 2
                    rotation: 45
                    color: "#3a6ea5"
                }
            }
        }
    }
}
