// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls

Popup {
    id: root
    modal: true
    focus: true
    closePolicy: Popup.NoAutoClose
    width: 280
    height: 200

    signal suitChosen(string suit)

    background: Rectangle {
        color: "#1c232c"
        radius: 12
        border.color: "#e0a94a"
        border.width: 2
    }

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Text {
            text: qsTr("You played a Jack — wish for a suit")
            color: "white"
            font.pixelSize: 15
            wrapMode: Text.WordWrap
            width: parent.width
        }

        Grid {
            columns: 2
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter

            Repeater {
                model: [
                    { suit: "Clubs", pip: "♣", red: false },
                    { suit: "Spades", pip: "♠", red: false },
                    { suit: "Hearts", pip: "♥", red: true },
                    { suit: "Diamonds", pip: "♦", red: true }
                ]
                delegate: Rectangle {
                    width: 110
                    height: 60
                    radius: 8
                    color: "#fbf8f2"
                    Text {
                        anchors.centerIn: parent
                        text: modelData.pip + " " + modelData.suit
                        font.pixelSize: 16
                        color: modelData.red ? "#c0392b" : "#1c232c"
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.suitChosen(modelData.suit)
                    }
                }
            }
        }
    }
}
