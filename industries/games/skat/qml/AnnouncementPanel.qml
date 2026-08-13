// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls

// Declarer's choice of game type. Null is intentionally absent here -- see
// README "Simplifications": SkatGame rejects it, so it is not offered.
Column {
    spacing: 10

    signal announce(string gameType, string trumpSuit)

    Text { color: "#f5f5f5"; font.pixelSize: 16; text: "Announce your game" }

    Text { color: "#c8d2e6"; text: "Suit game -- pick trump:" }
    Row {
        spacing: 8
        Repeater {
            model: [
                { code: "E", name: "Eichel" },
                { code: "G", name: "Gras" },
                { code: "H", name: "Herz" },
                { code: "S", name: "Schellen" },
            ]
            delegate: Button {
                text: modelData.name
                onClicked: announce("suit", modelData.code)
            }
        }
    }

    Button {
        text: "Grand (only the 4 Unters are trump)"
        onClicked: announce("grand", "")
    }
}
