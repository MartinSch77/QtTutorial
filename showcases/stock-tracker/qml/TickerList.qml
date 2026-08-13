// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls.Basic
import QtTutorial.StockTracker

Item {
    id: root
    required property StockTrackerBackend backend

    ListView {
        anchors.fill: parent
        model: backend.tickers
        clip: true
        spacing: 2

        delegate: Rectangle {
            width: ListView.view.width
            height: 48
            color: symbol === root.backend.selectedSymbol ? "#232a38" : "transparent"
            radius: 4

            MouseArea {
                anchors.fill: parent
                onClicked: root.backend.selectedSymbol = symbol
            }

            Row {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                spacing: 8

                Column {
                    width: parent.width * 0.4
                    anchors.verticalCenter: parent.verticalCenter
                    Text { text: symbol; color: "white"; font.bold: true; font.pixelSize: 15 }
                    Text { text: qsTr("$%1").arg(price.toFixed(2)); color: "#8fa0b3"; font.pixelSize: 12 }
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: (up ? "+" : "") + change.toFixed(2) + " (" + changePercent.toFixed(2) + "%)"
                    color: up ? "#3ddc84" : "#e5484d"
                    font.pixelSize: 13
                }
            }
        }
    }
}
