// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls.Basic
import QtTutorial.QuickControlsStyling

ApplicationWindow {
    id: window
    width: 480
    height: 640
    visible: true
    title: qsTr("Qt Quick Controls Styling")

    header: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton { text: qsTr("Dashboard") }
        TabButton { text: qsTr("Settings") }
        TabButton { text: qsTr("Styles") }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Page {
            title: qsTr("Dashboard")

            Column {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 24

                GroupBox {
                    title: qsTr("Fan Speed")
                    width: parent.width
                    height: 220

                    Column {
                        anchors.fill: parent
                        spacing: 8

                        Dial {
                            id: fanDial
                            anchors.horizontalCenter: parent.horizontalCenter
                            from: 0
                            to: 100
                            value: 40
                            width: 140
                            height: 140
                        }
                        Label {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: qsTr("%1%").arg(Math.round(fanDial.value))
                        }
                    }
                }

                GroupBox {
                    title: qsTr("Brightness")
                    width: parent.width
                    height: 80

                    Slider {
                        id: brightnessSlider
                        anchors.fill: parent
                        from: 0
                        to: 100
                        value: 70
                    }
                }

                GroupBox {
                    title: qsTr("Refresh Rate")
                    width: parent.width
                    height: 80

                    ComboBox {
                        anchors.fill: parent
                        model: [qsTr("30 Hz"), qsTr("60 Hz"), qsTr("120 Hz"), qsTr("144 Hz")]
                        currentIndex: 1
                    }
                }

                Button {
                    text: qsTr("Apply")
                    width: parent.width
                    highlighted: true
                }
            }
        }

        Page {
            title: qsTr("Settings")

            Column {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 4

                Label {
                    text: qsTr("Preferences")
                    font.bold: true
                    font.pixelSize: 16
                    bottomPadding: 12
                }

                SwitchDelegate {
                    text: qsTr("Enable notifications")
                    width: parent.width
                    checked: true
                }
                SwitchDelegate {
                    text: qsTr("Auto-update")
                    width: parent.width
                    checked: true
                }
                SwitchDelegate {
                    text: qsTr("Dark mode")
                    width: parent.width
                    checked: false
                }
                SwitchDelegate {
                    text: qsTr("Send diagnostics")
                    width: parent.width
                    checked: false
                }
            }
        }

        Page {
            title: qsTr("Styles")

            ListView {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 8
                model: StyleCatalogModel {}
                delegate: Column {
                    width: ListView.view.width
                    Label { text: name; font.bold: true }
                    Label { text: qsTr("Requires Qt %1 or later").arg(minimumQtVersion); font.pixelSize: 11 }
                    Label { text: note; wrapMode: Text.WordWrap; width: parent.width }
                }
            }
        }
    }
}
