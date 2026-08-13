// SPDX-License-Identifier: MIT
import QtQuick

// Renders the local player's hand. In "selectable" mode (Skat discard) up
// to maxSelection cards can be toggled and are reported via
// selectionChanged; otherwise a tap immediately plays the card.
Item {
    id: root
    height: 116
    width: row.width

    property var cards: []
    property bool selectable: false
    property int maxSelection: 2
    property var selectedCodes: []

    signal cardPlayed(string code)
    signal selectionChanged(var codes)

    function toggle(code) {
        var codes = selectedCodes.slice()
        var index = codes.indexOf(code)
        if (index >= 0) {
            codes.splice(index, 1)
        } else if (codes.length < maxSelection) {
            codes.push(code)
        }
        selectedCodes = codes
        selectionChanged(codes)
    }

    Row {
        id: row
        spacing: -18

        Repeater {
            model: root.cards
            delegate: CardView {
                cardCode: modelData
                selected: root.selectable && root.selectedCodes.indexOf(modelData) >= 0
                onClicked: root.selectable ? root.toggle(modelData) : root.cardPlayed(modelData)
            }
        }
    }
}
