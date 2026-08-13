// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls
import QtTutorial.Watten

ApplicationWindow {
    id: window
    width: 760
    height: 640
    visible: true
    title: qsTr("Watten – LAN Table")
    color: "#0d1b12"

    WattenTableController {
        id: controller
    }

    readonly property bool seated: controller.mySeat >= 0

    Rectangle {
        anchors.fill: parent
        visible: !seated
        color: "#0d1b12"

        Flickable {
            anchors.fill: parent
            anchors.margins: 24
            contentWidth: lobby.width
            contentHeight: lobby.height

            LobbyView {
                id: lobby
                controller: controller
            }
        }
    }

    Item {
        anchors.fill: parent
        visible: seated

        Rectangle {
            anchors.fill: parent
            color: "#123018"
        }

        Column {
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 12
            spacing: 4

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                font.pixelSize: 18
                font.bold: true
                text: controller.trumpSuit.length > 0
                    ? qsTr("Trump suit for this hand: %1").arg(controller.trumpSuit)
                    : qsTr("Trump not yet decided – waiting on opening lead")
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 24

                Text {
                    color: "#9fd6ff"
                    font.pixelSize: 14
                    text: qsTr("Team A (seats 0+2): %1").arg(controller.scoreTeamA)
                }
                Text {
                    color: "#ffb39f"
                    font.pixelSize: 14
                    text: qsTr("Team B (seats 1+3): %1").arg(controller.scoreTeamB)
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#f4c542"
                font.pixelSize: 13
                visible: controller.bummerlWinner.length > 0
                text: qsTr("Bummerl won by Team %1!").arg(controller.bummerlWinner)
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#8fa0b3"
                font.pixelSize: 12
                text: controller.statusMessage
            }
        }

        TrickArea {
            anchors.centerIn: parent
            trick: controller.trick
            turnSeat: controller.turnSeat
        }

        Repeater {
            model: 4
            delegate: Text {
                required property int index
                readonly property bool isMe: index === controller.mySeat
                readonly property bool isTurn: index === controller.turnSeat
                readonly property bool isDealer: index === controller.dealerSeat

                color: isTurn ? "#f4c542" : "white"
                font.pixelSize: 13
                font.bold: isTurn
                text: qsTr("Seat %1%2%3").arg(index)
                    .arg(isMe ? qsTr(" (you)") : "")
                    .arg(isDealer ? qsTr(" – leads") : "")

                x: {
                    switch (index) {
                    case 1: return window.width - width - 40;
                    case 3: return 40;
                    default: return (window.width - width) / 2;
                    }
                }
                y: {
                    switch (index) {
                    case 0: return window.height - 200;
                    case 2: return 90;
                    default: return window.height / 2 - 100;
                    }
                }
            }
        }

        PlayerHand {
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 24
            cards: controller.myHand
            myTurn: controller.turnSeat === controller.mySeat
            onCardClicked: (suit, rank) => controller.playCard(suit, rank)
        }
    }
}
