// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls

// Shown only to the declarer, right after picking up the Skat (hand is
// temporarily 12 cards): pick exactly 2 to bury back in the Skat.
Column {
    id: root
    spacing: 10

    property var hand: []

    signal discard(var codes)

    property var selection: []

    Text {
        color: "#f5f5f5"
        font.pixelSize: 16
        text: "You won the bid! Pick 2 cards to bury in the Skat (" + root.hand.length + "/12)"
    }

    HandView {
        cards: root.hand
        selectable: true
        maxSelection: 2
        onSelectionChanged: (codes) => root.selection = codes
    }

    Button {
        text: "Bury selected cards"
        enabled: root.selection.length === 2
        onClicked: root.discard(root.selection)
    }
}
