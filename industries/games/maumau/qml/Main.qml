// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls
import QtTutorial.MauMau

ApplicationWindow {
    id: window
    width: 900
    height: 640
    visible: true
    title: qsTr("Mau-Mau")
    color: "#101418"

    GameController {
        id: controller
    }

    // LobbyPage/TablePage each declare their own "property var controller" -
    // writing "controller: controller" directly in their object literals
    // resolves the unqualified RHS to that same component's own property
    // first (self-shadowing), not this outer "controller" id, producing a
    // genuine "Binding loop detected for property controller" and leaving
    // root.controller undefined at runtime (confirmed by actually running
    // the app: hostGame()/joinManual() failed with "Cannot call method ...
    // of undefined"). "window" has no property named "controller" itself,
    // so this capture is unambiguous, and "window.gameController" below is
    // unambiguous from any scope - same fix as the identical shape found in
    // industries/factory/offboard-digital-twin-control-center/qml/Main.qml.
    property var gameController: controller

    Loader {
        anchors.fill: parent
        sourceComponent: controller.inGame ? tablePageComponent : lobbyPageComponent
    }

    Component {
        id: lobbyPageComponent
        LobbyPage { controller: window.gameController }
    }

    Component {
        id: tablePageComponent
        TablePage { controller: window.gameController }
    }
}
