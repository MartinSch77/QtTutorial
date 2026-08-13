// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls
import QtTutorial.Kicker

ApplicationWindow {
    id: window
    width: 1024
    height: 720
    visible: true
    title: qsTr("Kicker – LAN Table Football")

    KickerController {
        id: controller
    }

    LobbyScreen {
        anchors.fill: parent
        controller: controller
        visible: controller.connectionStatus === "disconnected" || controller.connectionStatus.startsWith("error")
                 || controller.connectionStatus === "connecting"
    }

    GameScreen {
        anchors.fill: parent
        controller: controller
        visible: controller.connectionStatus === "hosting" || controller.connectionStatus === "connected"
    }
}
