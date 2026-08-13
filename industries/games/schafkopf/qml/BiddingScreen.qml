// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root
    property var bridge

    Column {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 24
        spacing: 12

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            font.pixelSize: 20
            text: bridge.isMyTurnToBid
                  ? qsTr("Your call: name an ace you don't hold, of a suit you have")
                  : qsTr("Seat %1 is deciding whether to call an ace...").arg(bridge.biddingSeat)
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10
            visible: bridge.isMyTurnToBid

            Repeater {
                model: bridge.legalCallSuits
                delegate: Button {
                    text: ({"E": qsTr("Call Eichel-Ass"), "G": qsTr("Call Gras-Ass"),
                            "S": qsTr("Call Schellen-Ass")})[modelData]
                    onClicked: bridge.callSuit(modelData)
                }
            }
            Button {
                text: qsTr("Pass")
                onClicked: bridge.pass()
            }
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            visible: bridge.isMyTurnToBid && bridge.legalCallSuits.length === 0
            text: qsTr("No legal call in this hand -- pass.")
            color: "#e0a94a"
        }
    }

    Row {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 24
        spacing: -20

        Repeater {
            model: bridge.myHand
            delegate: PlayingCard {
                suit: modelData.suit
                rank: modelData.rank
                legal: true
                dimmed: false
            }
        }
    }
}
