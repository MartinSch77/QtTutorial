// SPDX-License-Identifier: MIT
import QtQuick

// A hand-painted German-suited card face: no image assets, just shapes and
// text. Suit symbols borrow the familiar French-suit glyphs as a stand-in
// (Eichel~Clubs, Gras~Spades, Herz~Hearts, Schellen~Diamonds) purely so the
// symbols render reliably with any font -- see README "Suit naming" for why.
Item {
    id: root
    width: 76
    height: 108

    property string cardCode: ""
    property bool faceUp: true
    property bool selected: false
    property bool playable: true

    signal clicked()

    readonly property string suitLetter: cardCode.length > 0 ? cardCode.charAt(0) : ""
    readonly property string rankLetter: cardCode.length > 1 ? cardCode.charAt(1) : ""

    readonly property var suitInfo: ({
        "E": { symbol: "♣", color: "#6b4226", name: "Eichel" },
        "G": { symbol: "♠", color: "#2f7d32", name: "Gras" },
        "H": { symbol: "♥", color: "#b71c1c", name: "Herz" },
        "S": { symbol: "♦", color: "#c98a12", name: "Schellen" },
    }[suitLetter] || { symbol: "?", color: "#333333", name: "" })

    readonly property string rankText: ({
        "7": "7", "8": "8", "9": "9", "T": "10",
        "U": "U", "O": "O", "K": "K", "A": "A",
    }[rankLetter] || "")

    y: selected ? -16 : 0
    Behavior on y { NumberAnimation { duration: 140; easing.type: Easing.OutQuad } }
    scale: hoverArea.containsMouse && playable ? 1.05 : 1.0
    Behavior on scale { NumberAnimation { duration: 100 } }

    Rectangle {
        anchors.fill: parent
        radius: 8
        color: faceUp ? "#fdfaf3" : "#28324a"
        border.color: root.selected ? "#f4c430" : "#20304a"
        border.width: root.selected ? 3 : 1

        Column {
            visible: faceUp
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 5
            spacing: -2
            Text { text: root.rankText; color: root.suitInfo.color; font.pixelSize: 16; font.bold: true }
            Text { text: root.suitInfo.symbol; color: root.suitInfo.color; font.pixelSize: 14 }
        }

        Text {
            visible: faceUp
            anchors.centerIn: parent
            text: root.suitInfo.symbol
            color: root.suitInfo.color
            font.pixelSize: 34
            opacity: 0.85
        }

        Column {
            visible: faceUp
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 5
            spacing: -2
            rotation: 180
            Text { text: root.rankText; color: root.suitInfo.color; font.pixelSize: 16; font.bold: true }
            Text { text: root.suitInfo.symbol; color: root.suitInfo.color; font.pixelSize: 14 }
        }

        Rectangle {
            visible: !faceUp
            anchors.fill: parent
            anchors.margins: 6
            radius: 4
            color: "transparent"
            border.color: "#4c6690"
            border.width: 2
        }
    }

    MouseArea {
        id: hoverArea
        anchors.fill: parent
        hoverEnabled: true
        enabled: root.playable
        onClicked: root.clicked()
    }
}
