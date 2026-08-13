// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtTutorial.StockTracker

ApplicationWindow {
    id: window
    width: 1100
    height: 700
    minimumWidth: 380
    minimumHeight: 560
    visible: true
    title: qsTr("Stock Tracker – Trading Cockpit (Qt showcase)")
    color: "#0e1116"

    StockTrackerBackend {
        id: backend
    }

    // Responsive breakpoint: below this width the cockpit stacks into a single
    // column (ticker list, then detail+chart, then order panel); at or above it,
    // the three panels sit side by side. GridLayout's `columns` is re-evaluated
    // live as the window is resized, so this reflows continuously rather than
    // being fixed at startup.
    readonly property bool isWide: width >= 820

    GridLayout {
        anchors.fill: parent
        anchors.margins: 16
        columns: window.isWide ? 3 : 1
        columnSpacing: 16
        rowSpacing: 16

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: window.isWide ? 260 : -1
            Layout.minimumHeight: 180
            color: "#171b23"
            radius: 8

            Column {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 6

                Text { text: qsTr("Watchlist"); color: "white"; font.bold: true; font.pixelSize: 14 }

                TickerList {
                    width: parent.width
                    height: parent.height - 26
                    backend: backend
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 320
            color: "#171b23"
            radius: 8

            DetailPanel {
                anchors.fill: parent
                anchors.margins: 12
                backend: backend
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: window.isWide ? 300 : -1
            Layout.minimumHeight: 260
            color: "#171b23"
            radius: 8

            OrderPanel {
                anchors.fill: parent
                anchors.margins: 12
                backend: backend
            }
        }
    }
}
