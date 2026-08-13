// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtTutorial.Defence.Onboard

// Kiosk-style multi-platform HMI: a single onboard shell with a mode
// switcher across four passive display panels (vehicle crew status, sensor
// track display, drone object-detection overlay, AR HUD). In a real
// embedded deployment this window would be shown fullscreen with no window
// decorations; it is left windowed here so the demo runs on a desktop.
Window {
    id: window
    width: 1000
    height: 640
    visible: true
    color: "#0c0f14"
    title: qsTr("QtTutorial - Multi-Platform HMI")

    PlatformSimulator {
        id: sim
    }

    TabBar {
        id: tabBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        TabButton { text: qsTr("Vehicle Crew Display") }
        TabButton { text: qsTr("Sensor Track Display") }
        TabButton { text: qsTr("Drone Object Detection") }
        TabButton { text: qsTr("AR HUD Overlay") }
        TabButton { text: qsTr("Comms & Data Link") }
    }

    Item {
        id: panelHost
        anchors.top: tabBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        VehicleStatusPanel { anchors.fill: parent; sim: sim; visible: tabBar.currentIndex === 0 }
        TrackDisplayPanel { anchors.fill: parent; sim: sim; visible: tabBar.currentIndex === 1 }
        DetectionPanel { anchors.fill: parent; sim: sim; visible: tabBar.currentIndex === 2 }
        HudPanel { anchors.fill: parent; sim: sim; visible: tabBar.currentIndex === 3 }
        CommsStatusPanel { anchors.fill: parent; sim: sim; visible: tabBar.currentIndex === 4 }
    }
}
