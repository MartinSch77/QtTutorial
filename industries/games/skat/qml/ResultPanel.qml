// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls

// End-of-hand result. Deliberately simplified: reports win/lose and the raw
// point split rather than the full traditional game-value scoring table --
// see README "Simplifications".
Column {
    id: root
    spacing: 10

    property var resultInfo: ({})
    property bool isHost: false

    signal nextHand()

    readonly property bool win: resultInfo.win === true

    Text {
        color: root.win ? "#7ee787" : "#ff8080"
        font.pixelSize: 20
        font.bold: true
        text: "Declarer (seat " + root.resultInfo.declarerSeat + ") " + (root.win ? "WON" : "LOST") + " the hand"
    }

    Text {
        color: "#f5f5f5"
        text: "Declarer points: " + root.resultInfo.declarerPoints + " / 120"
    }
    Text {
        color: "#f5f5f5"
        text: "Defender points: " + root.resultInfo.defenderPoints + " / 120"
    }
    Text {
        color: "#c8d2e6"
        text: "(61+ points needed to win, including the buried Skat cards)"
    }

    Button {
        visible: root.isHost
        text: "Deal next hand"
        onClicked: root.nextHand()
    }
}
