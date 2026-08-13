// SPDX-License-Identifier: MIT
import QtQuick

Item {
    id: root
    property var bridge

    readonly property int tricksCompletedApprox: Math.max(0, 8 - bridge.myHand.length)

    function seatLabel(seat) {
        if (seat === bridge.mySeat) {
            return qsTr("You (seat %1)").arg(seat)
        }
        return qsTr("Seat %1").arg(seat)
    }

    Text {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 12
        color: "white"
        font.pixelSize: 16
        text: bridge.callerSeat >= 0
              ? qsTr("Seat %1 called the %2-Ass. %3")
                    .arg(bridge.callerSeat)
                    .arg(bridge.calledSuitId)
                    .arg(bridge.isMyTurnToPlay ? qsTr("Your turn -- pick a highlighted card.")
                                                : qsTr("Seat %1 to play.").arg(bridge.turnSeat))
              : ""
    }

    Row {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 12
        spacing: 16
        Repeater {
            model: 4
            delegate: Column {
                spacing: 2
                visible: modelData !== bridge.mySeat
                Text {
                    text: root.seatLabel(modelData)
                    color: bridge.turnSeat === modelData ? "#3ddc84" : "#8fa0b3"
                    font.pixelSize: 12
                    font.bold: bridge.turnSeat === modelData
                }
                Row {
                    spacing: -46
                    Repeater {
                        model: Math.max(0, 8 - root.tricksCompletedApprox - (bridge.turnSeat === modelData ? 0 : 0))
                        delegate: CardBack {
                            scale: 0.55
                        }
                    }
                }
            }
        }
    }

    Item {
        id: trickPile
        anchors.centerIn: parent
        width: 220
        height: 160

        Repeater {
            model: bridge.trick
            delegate: Item {
                readonly property real angle: (modelData.seat / 4.0) * 2 * Math.PI
                x: trickPile.width / 2 - 38 + Math.cos(angle) * 30
                y: trickPile.height / 2 - 54 + Math.sin(angle) * 30
                scale: 0.0
                Component.onCompleted: entrance.start()

                NumberAnimation { id: entrance; target: parent; property: "scale"; to: 1.0; duration: 220; easing.type: Easing.OutBack }

                PlayingCard {
                    suit: modelData.id.charAt(0) === "E" ? "Eichel" : modelData.id.charAt(0) === "G" ? "Gras"
                          : modelData.id.charAt(0) === "H" ? "Herz" : "Schellen"
                    rank: ({"7": "Sieben", "8": "Acht", "9": "Neun", "Z": "Zehn",
                             "U": "Unter", "O": "Ober", "K": "Koenig", "A": "Ass"})[modelData.id.charAt(1)]
                    legal: true
                }
            }
        }
    }

    Row {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 20
        spacing: -20

        Repeater {
            model: bridge.myHand
            delegate: PlayingCard {
                suit: modelData.suit
                rank: modelData.rank
                legal: modelData.legal
                dimmed: !modelData.legal

                MouseArea {
                    anchors.fill: parent
                    enabled: modelData.legal && bridge.isMyTurnToPlay
                    onClicked: bridge.playCard(modelData.id)
                }
            }
        }
    }
}
