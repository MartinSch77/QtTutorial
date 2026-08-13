// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls

Column {
    id: root

    property var controller

    spacing: 16
    width: 360

    Text {
        text: qsTr("Watten – Tyrolean-style 4-player partnership")
        color: "white"
        font.pixelSize: 20
        font.bold: true
        wrapMode: Text.WordWrap
        width: root.width
    }

    Text {
        text: qsTr("Simplified digital rules – see README for the exact variant and omissions (no Zeichengeben).")
        color: "#8fa0b3"
        font.pixelSize: 12
        wrapMode: Text.WordWrap
        width: root.width
    }

    GroupBox {
        title: qsTr("Host a table")
        width: root.width

        Column {
            spacing: 8
            width: parent.width

            TextField {
                id: hostNameField
                width: parent.width
                placeholderText: qsTr("Your name (shown to LAN browsers)")
                text: qsTr("Watten table")
            }

            Button {
                text: qsTr("Host (empty seats filled by bots)")
                onClicked: {
                    root.controller.hostGame(hostNameField.text);
                    root.controller.startTable();
                }
            }
        }
    }

    GroupBox {
        title: qsTr("Join over LAN")
        width: root.width

        Column {
            spacing: 8
            width: parent.width

            Button {
                text: qsTr("Search LAN")
                onClicked: root.controller.browseLan()
            }

            Repeater {
                model: root.controller.lanGames
                delegate: Button {
                    required property var modelData
                    required property int index
                    width: parent.width
                    text: qsTr("%1 (%2/%3 seats) – %4:%5")
                        .arg(modelData.hostName)
                        .arg(modelData.seatsTaken)
                        .arg(modelData.seatsTotal)
                        .arg(modelData.hostAddress)
                        .arg(modelData.tcpPort)
                    onClicked: root.controller.joinDiscovered(index)
                }
            }
        }
    }

    GroupBox {
        title: qsTr("Join by address")
        width: root.width

        Row {
            spacing: 8

            TextField {
                id: addressField
                placeholderText: qsTr("Host address")
                text: "127.0.0.1"
            }

            TextField {
                id: portField
                placeholderText: qsTr("Port")
                validator: IntValidator { bottom: 1; top: 65535 }
            }

            Button {
                text: qsTr("Join")
                onClicked: root.controller.joinManual(addressField.text, parseInt(portField.text))
            }
        }
    }

    Text {
        text: root.controller.statusMessage
        color: "#f4c542"
        wrapMode: Text.WordWrap
        width: root.width
    }
}
