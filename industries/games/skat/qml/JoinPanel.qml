// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// The lobby screen: host a new table, or discover/join one on the LAN
// (with a manual host:port fallback for networks where UDP broadcast is
// blocked).
ColumnLayout {
    id: root
    spacing: 14

    property var discoveredGames: []

    signal hostRequested(string name)
    signal refreshRequested()
    signal joinDiscoveredRequested(int index, string name)
    signal joinManualRequested(string host, int port, string name)

    Text {
        text: "Skat -- 3 of up to 4 players over LAN"
        color: "#f5f5f5"
        font.pixelSize: 20
        font.bold: true
    }

    RowLayout {
        Text { text: "Your name:"; color: "#c8d2e6" }
        TextField {
            id: nameField
            text: "Player"
            Layout.preferredWidth: 160
        }
    }

    Button {
        text: "Host a new table"
        onClicked: root.hostRequested(nameField.text)
    }

    Rectangle { Layout.fillWidth: true; height: 1; color: "#3a4a68" }

    RowLayout {
        Text { text: "Games found on this LAN:"; color: "#c8d2e6" }
        Button { text: "Refresh"; onClicked: root.refreshRequested() }
    }

    ListView {
        Layout.preferredWidth: 360
        Layout.preferredHeight: 120
        model: root.discoveredGames
        delegate: RowLayout {
            width: ListView.view.width
            Text {
                color: "#f5f5f5"
                Layout.fillWidth: true
                text: modelData.hostName + " (" + modelData.hostAddress + ":" + modelData.tcpPort + ") -- "
                      + modelData.seatsTaken + "/" + modelData.seatsTotal + " seated"
            }
            Button {
                text: "Join"
                onClicked: root.joinDiscoveredRequested(index, nameField.text)
            }
        }
    }

    Rectangle { Layout.fillWidth: true; height: 1; color: "#3a4a68" }

    RowLayout {
        Text { text: "Or join manually:"; color: "#c8d2e6" }
        TextField { id: hostField; placeholderText: "host"; Layout.preferredWidth: 140 }
        TextField { id: portField; placeholderText: "port"; Layout.preferredWidth: 80 }
        Button {
            text: "Join"
            onClicked: root.joinManualRequested(hostField.text, parseInt(portField.text, 10) || 0, nameField.text)
        }
    }
}
