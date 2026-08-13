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

    // LobbyScreen/GameScreen each declare their own "required property var
    // controller" - writing "controller: controller" below would self-
    // shadow to that property instead of this outer id, producing a
    // binding loop and leaving each screen's controller undefined at
    // runtime (same bug shape found and fixed in
    // industries/games/maumau/qml/Main.qml,
    // industries/games/watten/qml/Main.qml, and
    // industries/factory/offboard-digital-twin-control-center/qml/Main.qml).
    property var gameController: controller

    LobbyScreen {
        anchors.fill: parent
        controller: window.gameController
        visible: controller.connectionStatus === "disconnected" || controller.connectionStatus.startsWith("error")
                 || controller.connectionStatus === "connecting"
    }

    GameScreen {
        anchors.fill: parent
        controller: window.gameController
        visible: controller.connectionStatus === "hosting" || controller.connectionStatus === "connected"
    }
}
