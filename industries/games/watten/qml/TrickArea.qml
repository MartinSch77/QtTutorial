// SPDX-License-Identifier: MIT
import QtQuick

Item {
    id: root

    property var trick: []
    property int turnSeat: -1

    width: 220
    height: 220

    function cardForSeat(seat) {
        for (let i = 0; i < trick.length; ++i) {
            if (trick[i].seat === seat) {
                return trick[i];
            }
        }
        return null;
    }

    Repeater {
        model: 4
        delegate: Item {
            required property int index
            readonly property var played: root.cardForSeat(index)
            readonly property bool isTurn: root.turnSeat === index

            width: 70
            height: 96
            x: {
                switch (index) {
                case 1: return root.width - width;
                case 3: return 0;
                default: return (root.width - width) / 2;
                }
            }
            y: {
                switch (index) {
                case 0: return root.height - height;
                case 2: return 0;
                default: return (root.height - height) / 2;
                }
            }

            Rectangle {
                anchors.fill: parent
                radius: 8
                color: "transparent"
                border.color: isTurn ? "#f4c542" : "#445566"
                border.width: isTurn ? 2 : 1
                opacity: 0.6
            }

            CardFace {
                anchors.centerIn: parent
                visible: played !== null
                suit: played ? played.suit : "Herz"
                rank: played ? played.rank : "S"

                Behavior on opacity { NumberAnimation { duration: 200 } }
                Behavior on scale { NumberAnimation { duration: 200; easing.type: Easing.OutBack } }

                opacity: played !== null ? 1 : 0
                scale: played !== null ? 1 : 0.4
            }
        }
    }
}
