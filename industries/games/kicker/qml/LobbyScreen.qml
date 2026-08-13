// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// The pre-game screen: host a new match, browse LAN games discovered via
// LanBrowser (gameId "kicker"), or fall back to typing a host address
// manually.
Item {
    id: root

    required property var controller

    Rectangle {
        anchors.fill: parent
        color: "#101418"
    }

    ColumnLayout {
        anchors.centerIn: parent
        width: 380
        spacing: 20

        Text {
            text: qsTr("Kicker – LAN Table Football")
            color: "white"
            font.pixelSize: 22
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        GroupBox {
            title: qsTr("Host a match")
            Layout.fillWidth: true
            ColumnLayout {
                anchors.fill: parent
                RowLayout {
                    Label { text: qsTr("Players (2-4):"); color: "white" }
                    SpinBox { id: seatSpinBox; from: 2; to: 4; value: 2 }
                }
                Button {
                    text: qsTr("Host")
                    Layout.fillWidth: true
                    onClicked: root.controller.hostMatch(seatSpinBox.value)
                }
            }
        }

        GroupBox {
            title: qsTr("Join a discovered game")
            Layout.fillWidth: true
            ColumnLayout {
                anchors.fill: parent
                ListView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    model: root.controller.lobbyGames
                    delegate: Button {
                        width: ListView.view.width
                        text: qsTr("%1 (%2/%3 seats)").arg(modelData.hostName).arg(modelData.seatsTaken).arg(modelData.seatsTotal)
                        onClicked: root.controller.joinDiscovered(index)
                    }
                }
            }
        }

        GroupBox {
            title: qsTr("Join by address")
            Layout.fillWidth: true
            RowLayout {
                anchors.fill: parent
                TextField { id: hostField; placeholderText: qsTr("host address"); Layout.fillWidth: true }
                TextField { id: portField; placeholderText: qsTr("port"); Layout.preferredWidth: 70 }
                Button {
                    text: qsTr("Join")
                    onClicked: root.controller.joinManual(hostField.text, parseInt(portField.text, 10) || 0)
                }
            }
        }

        Text {
            text: qsTr("Status: %1").arg(root.controller.connectionStatus)
            color: "#8fa0b3"
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
