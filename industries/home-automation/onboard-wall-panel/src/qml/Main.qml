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
    readonly property var tabLabels: [qsTr("Scenes"), qsTr("Lighting"), qsTr("Climate"), qsTr("Locks"), qsTr("Blinds"), qsTr("Security")]

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

                Row {
                    spacing: 8
                    anchors.left: parent.left
                    anchors.leftMargin: 4

                    HouseIcon {
                        width: 22
                        height: 22
                        anchors.verticalCenter: parent.verticalCenter
                        strokeColor: "#39c0ff"
                    }
                    Text {
                        text: qsTr("HOME")
                        color: "#39c0ff"
                        font.pixelSize: 20
                        font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                    }
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
            id: contentArea
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
                    // A switch rather than an array literal indexed by
                    // currentTab: the array-literal form reads every
                    // panel's height on every evaluation (to build the
                    // array) even though only one is ever used, which
                    // multiplies re-entrant evaluations as each panel's
                    // Repeater/Grid settles its own implicit height and
                    // trips Qt Quick's binding-loop detector. The switch
                    // only reads the one panel's height that is actually
                    // selected.
                    //
                    // Not "parent.height": inside a Flickable, an item's
                    // implicit QML "parent" is the Flickable's
                    // auto-created contentItem, whose height Qt Quick
                    // binds back to Flickable.contentHeight - which line
                    // 87 above sets to *this* item's height. Referencing
                    // "parent.height" here closed that loop directly
                    // (contentItem.height -> contentHeight -> this height
                    // -> parent.height -> contentItem.height again),
                    // reproducing regardless of which tab/panel was
                    // active - confirmed by actually running this app.
                    // "contentArea" (the Flickable's own real, non-
                    // circular ancestor two levels up) breaks the cycle.
                    height: Math.max(contentArea.height, currentPanelHeight())

                    function currentPanelHeight() {
                        switch (window.currentTab) {
                        case 0:
                            return scenesPanel.height;
                        case 1:
                            return lightingPanel.height;
                        case 2:
                            return climatePanel.height;
                        case 3:
                            return locksPanel.height;
                        case 4:
                            return blindsPanel.height;
                        default:
                            return securityPanel.height;
                        }
                    }

                    ScenesPanel {
                        id: scenesPanel
                        width: parent.width
                        visible: window.currentTab === 0
                        roomModel: roomModel
                        lockModel: lockModel
                        securityModel: securityModel
                        thermostat: thermostat
                    }
                    LightingPanel {
                        id: lightingPanel
                        width: parent.width
                        visible: window.currentTab === 1
                        roomModel: roomModel
                    }
                    ClimatePanel {
                        id: climatePanel
                        width: parent.width
                        visible: window.currentTab === 2
                        thermostat: thermostat
                    }
                    LocksPanel {
                        id: locksPanel
                        width: parent.width
                        visible: window.currentTab === 3
                        lockModel: lockModel
                    }
                    BlindsPanel {
                        id: blindsPanel
                        width: parent.width
                        visible: window.currentTab === 4
                        roomModel: roomModel
                    }
                    SecurityPanel {
                        id: securityPanel
                        width: parent.width
                        visible: window.currentTab === 5
                        securityModel: securityModel
                    }
                }
            }
        }
    }
}
