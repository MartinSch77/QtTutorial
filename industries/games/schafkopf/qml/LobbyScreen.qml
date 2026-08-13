// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root
    property var bridge

    Column {
        anchors.centerIn: parent
        width: 340
        spacing: 16

        Text {
            text: qsTr("Schafkopf – Rufspiel")
            color: "white"
            font.pixelSize: 24
            font.bold: true
        }

        Text {
            text: bridge.statusText
            color: "#8fa0b3"
            width: parent.width
            wrapMode: Text.WordWrap
        }

        TextField {
            id: nameField
            width: parent.width
            placeholderText: qsTr("Your name")
            text: qsTr("Player")
        }

        Rectangle { width: parent.width; height: 1; color: "#2a3540" }

        Text { text: qsTr("Host a table"); color: "white"; font.bold: true }
        Button {
            width: parent.width
            text: qsTr("Host new game")
            visible: bridge.stage === "disconnected"
            onClicked: bridge.hostGame(nameField.text)
        }
        Button {
            width: parent.width
            text: qsTr("Start dealing (fills empty seats with bots)")
            visible: bridge.isHost && bridge.stage === "lobby"
            onClicked: bridge.startPlaying()
        }

        Rectangle { width: parent.width; height: 1; color: "#2a3540"; visible: bridge.stage === "disconnected" }

        Text { text: qsTr("Or join one on the LAN"); color: "white"; font.bold: true; visible: bridge.stage === "disconnected" }
        Button {
            width: parent.width
            text: qsTr("Search for games")
            visible: bridge.stage === "disconnected"
            onClicked: bridge.startDiscovery()
        }
        Repeater {
            model: bridge.discoveredGames
            delegate: Button {
                width: 340
                text: qsTr("Join %1 (%2/%3 seats)").arg(modelData.hostName).arg(modelData.seatsTaken).arg(modelData.seatsTotal)
                onClicked: bridge.joinDiscovered(index, nameField.text)
            }
        }

        Row {
            width: parent.width
            spacing: 8
            visible: bridge.stage === "disconnected"
            TextField { id: hostField; width: 180; placeholderText: qsTr("IP address") }
            TextField { id: portField; width: 80; placeholderText: qsTr("port"); validator: IntValidator { bottom: 1; top: 65535 } }
            Button {
                width: 60
                text: qsTr("Join")
                onClicked: bridge.joinGame(hostField.text, parseInt(portField.text || "0"), nameField.text)
            }
        }
    }
}
