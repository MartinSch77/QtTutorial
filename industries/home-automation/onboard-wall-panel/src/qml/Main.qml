// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Window
import QtTutorial.HomeAutomation.Onboard

// Kiosk-style wall-mounted smart-home control panel: one application
// controlling several device categories via a sidebar-selected panel. In a
// real installation this would run with visibility: Window.FullScreen and
// no window decorations, directly on the wall unit's touch screen with no
// network connection; it is left windowed here so the demo runs on a
// regular desktop.
Window {
    id: window
    width: 1000
    height: 620
    visible: true
    color: "#0d1117"
    title: qsTr("QtTutorial - Home Wall Panel")

    RoomListModel { id: roomModel }
    LockListModel { id: lockModel }
    SecurityListModel { id: securityModel }
    ThermostatController { id: thermostat }

    property int currentTab: 0
    readonly property var tabLabels: [qsTr("Lighting"), qsTr("Climate"), qsTr("Locks"), qsTr("Blinds"), qsTr("Security")]

    Row {
        anchors.fill: parent

        Rectangle {
            id: sidebar
            width: 180
            height: parent.height
            color: "#141a24"

            Column {
                anchors.top: parent.top
                anchors.topMargin: 24
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                spacing: 8

                Text {
                    text: qsTr("HOME")
                    color: "#39c0ff"
                    font.pixelSize: 20
                    font.bold: true
                    anchors.left: parent.left
                    anchors.leftMargin: 4
                }

                Item { width: 1; height: 12 }

                Repeater {
                    model: window.tabLabels
                    SidebarButton {
                        label: modelData
                        active: window.currentTab === index
                        onClicked: window.currentTab = index
                    }
                }
            }
        }

        Item {
            width: window.width - sidebar.width
            height: parent.height

            Flickable {
                anchors.fill: parent
                anchors.margins: 28
                contentWidth: width
                contentHeight: content.height
                clip: true

                Item {
                    id: content
                    width: parent.width
                    height: Math.max(parent.height,
                                      [lightingPanel, climatePanel, locksPanel, blindsPanel, securityPanel][window.currentTab].height)

                    LightingPanel {
                        id: lightingPanel
                        width: parent.width
                        visible: window.currentTab === 0
                        roomModel: roomModel
                    }
                    ClimatePanel {
                        id: climatePanel
                        width: parent.width
                        visible: window.currentTab === 1
                        thermostat: thermostat
                    }
                    LocksPanel {
                        id: locksPanel
                        width: parent.width
                        visible: window.currentTab === 2
                        lockModel: lockModel
                    }
                    BlindsPanel {
                        id: blindsPanel
                        width: parent.width
                        visible: window.currentTab === 3
                        roomModel: roomModel
                    }
                    SecurityPanel {
                        id: securityPanel
                        width: parent.width
                        visible: window.currentTab === 4
                        securityModel: securityModel
                    }
                }
            }
        }
    }
}
