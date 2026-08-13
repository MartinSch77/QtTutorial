// SPDX-License-Identifier: MIT
import QtQuick

Row {
    id: root

    property var cards: []
    property bool myTurn: false
    signal cardClicked(string suit, string rank)

    spacing: 8

    Repeater {
        model: root.cards
        delegate: CardFace {
            required property var modelData
            suit: modelData.suit
            rank: modelData.rank
            highlighted: root.myTurn

            Behavior on y { NumberAnimation { duration: 150 } }

            MouseArea {
                anchors.fill: parent
                enabled: root.myTurn
                cursorShape: root.myTurn ? Qt.PointingHandCursor : Qt.ArrowCursor
                onClicked: root.cardClicked(modelData.suit, modelData.rank)
            }
        }
    }
}
