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

    Loader {
        anchors.fill: parent
        sourceComponent: controller.inGame ? tablePageComponent : lobbyPageComponent
    }

    Component {
        id: lobbyPageComponent
        LobbyPage { controller: controller }
    }

    Component {
        id: tablePageComponent
        TablePage { controller: controller }
    }
}
