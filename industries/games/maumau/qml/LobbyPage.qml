// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls

Item {
    id: root
    property var controller

    Column {
        anchors.centerIn: parent
        width: 360
        spacing: 18

        Text {
            text: qsTr("Mau-Mau")
            color: "white"
            font.pixelSize: 30
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            visible: root.controller.connectionError.length > 0
            text: root.controller.connectionError
            color: "#e0524a"
            wrapMode: Text.WordWrap
            width: parent.width
        }

        Rectangle {
            width: parent.width
            height: 1
            color: "#33404d"
        }

        Text { text: qsTr("Host a new game"); color: "#8fa0b3"; font.pixelSize: 14 }

        Row {
            width: parent.width
            spacing: 8
            TextField {
                id: hostNameField
                width: parent.width - joinButton.width - 8
                placeholderText: qsTr("Your name")
            }
            Button {
                id: joinButton
                text: qsTr("Host")
                onClicked: root.controller.hostGame(hostNameField.text)
            }
        }

        Rectangle {
            width: parent.width
            height: 1
            color: "#33404d"
        }

        Text { text: qsTr("Join a game on this LAN"); color: "#8fa0b3"; font.pixelSize: 14 }

        ListView {
            width: parent.width
            height: 120
            clip: true
            model: root.controller.discoveredGames
            delegate: Rectangle {
                width: ListView.view.width
                height: 44
                color: "#1c232c"
                radius: 6
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.margins: 10
                    color: "white"
                    text: modelData.hostName + " (" + modelData.seatsTaken + "/" + modelData.seatsTotal + ")"
                }
                Button {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.margins: 6
                    text: qsTr("Join")
                    onClicked: root.controller.joinDiscovered(index)
                }
            }
        }

        Row {
            width: parent.width
            spacing: 8
            TextField {
                id: addressField
                width: 160
                placeholderText: qsTr("Host address")
                text: "127.0.0.1"
            }
            TextField {
                id: portField
                width: 80
                placeholderText: qsTr("Port")
                validator: IntValidator { bottom: 1; top: 65535 }
            }
            Button {
                text: qsTr("Join by IP")
                enabled: addressField.text.length > 0 && portField.text.length > 0
                onClicked: root.controller.joinManual(addressField.text, parseInt(portField.text))
            }
        }
    }
}
