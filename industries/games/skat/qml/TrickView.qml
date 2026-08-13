// SPDX-License-Identifier: MIT
import QtQuick

// The cards currently lying on the table for the trick in progress.
Row {
    id: root
    spacing: 18

    property var trickCards: [] // [{seat, card}]

    Repeater {
        model: root.trickCards
        delegate: Column {
            spacing: 4
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Seat " + modelData.seat
                color: "#dfe6f2"
                font.pixelSize: 12
            }
            CardView {
                cardCode: modelData.card
                playable: false

                Component.onCompleted: {
                    opacity = 0
                    opacity = 1
                }
                Behavior on opacity { NumberAnimation { duration: 220 } }
                Behavior on scale { NumberAnimation { duration: 220; easing.type: Easing.OutBack } }
            }
        }
    }
}
