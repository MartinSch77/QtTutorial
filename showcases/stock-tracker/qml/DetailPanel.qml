// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls.Basic
import QtTutorial.StockTracker

Item {
    id: root
    required property StockTrackerBackend backend

    implicitHeight: column.implicitHeight

    Column {
        id: column
        anchors.fill: parent
        spacing: 8

        Row {
            width: parent.width
            spacing: 10

            Text {
                text: root.backend.selectedSymbol
                color: "white"
                font.pixelSize: 22
                font.bold: true
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("$%1").arg(root.backend.selectedPrice.toFixed(2))
                color: "#4ac1e0"
                font.pixelSize: 18
            }

            Image {
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/stock_tracker/icons/feed-status.svg"
                width: 18
                height: 18
                opacity: root.backend.feedConnected ? 1.0 : 0.35
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: root.backend.feedConnected ? qsTr("feed connected") : qsTr("feed offline")
                color: "#5a6b80"
                font.pixelSize: 11
            }
        }

        Loader {
            id: chartLoader
            width: parent.width
            height: 260
            source: root.backend.graphsAvailable
                ? "PriceChartGraphs.qml"
                : "PriceChartFallback.qml"

            onLoaded: {
                item.priceHistory = root.backend.selectedPriceHistory;
            }
        }

        Connections {
            target: root.backend
            function onSelectedPriceHistoryChanged() {
                if (chartLoader.item) {
                    chartLoader.item.priceHistory = root.backend.selectedPriceHistory;
                }
            }
        }

        Text {
            text: root.backend.feedMode
            color: "#5a6b80"
            font.pixelSize: 10
            wrapMode: Text.WordWrap
            width: parent.width
        }
    }
}
