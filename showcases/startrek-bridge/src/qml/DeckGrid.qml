// SPDX-License-Identifier: MIT
import QtQuick

// Reads a QVariantList of {name, state} entries (state is "Nominal",
// "Caution" or "Critical", as produced by DeckLifeSupportMachine) and draws
// one colored cell per deck - a life-support status grid, not a random
// flicker board: each cell's color only changes when the underlying state
// machine actually transitions.
Grid {
    id: root
    property var decks: []
    columns: Math.max(1, Math.floor((width + spacing) / (130 + spacing)))
    spacing: 10

    function colorForState(state) {
        if (state === "Critical") return "#c0392b";
        if (state === "Caution") return "#e0a300";
        return "#2fa84f";
    }

    Repeater {
        model: root.decks
        Rectangle {
            width: 130
            height: 60
            radius: 6
            color: root.colorForState(modelData.state)

            Behavior on color { ColorAnimation { duration: 300 } }

            Column {
                anchors.centerIn: parent
                spacing: 2
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: modelData.name
                    color: "white"
                    font.bold: true
                    font.pixelSize: 12
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: modelData.state
                    color: "white"
                    font.pixelSize: 11
                }
            }
        }
    }
}
