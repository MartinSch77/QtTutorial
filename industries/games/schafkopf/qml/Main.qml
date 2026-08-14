// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls.Basic
import QtTutorial.Schafkopf

ApplicationWindow {
    id: window
    width: 900
    height: 640
    visible: true
    title: qsTr("Schafkopf – Rufspiel")
    color: "#0d3b1e"

    GameBridge {
        id: bridge
    }

    // LobbyScreen/BiddingScreen/PlayingScreen/ResultScreen each declare
    // their own "property var bridge" - writing "bridge: bridge" directly
    // in their object literals below self-shadows to that same property
    // instead of this outer GameBridge id, producing a binding loop and
    // leaving each screen's bridge undefined at runtime (the same bug
    // shape found and fixed in industries/games/maumau/qml/Main.qml,
    // industries/games/watten/qml/Main.qml,
    // industries/games/kicker/qml/Main.qml, and
    // industries/factory/offboard-digital-twin-control-center/qml/Main.qml
    // - confirmed here by a repo-wide static sweep for the same pattern,
    // not by a report of this specific game failing).
    property var gameBridge: bridge

    LobbyScreen {
        anchors.fill: parent
        bridge: window.gameBridge
        visible: bridge.stage !== "playing"
    }

    BiddingScreen {
        anchors.fill: parent
        bridge: window.gameBridge
        visible: bridge.stage === "playing" && bridge.handPhase === "bidding"
    }

    PlayingScreen {
        anchors.fill: parent
        bridge: window.gameBridge
        visible: bridge.stage === "playing" && bridge.handPhase === "playing"
    }

    ResultScreen {
        anchors.fill: parent
        bridge: window.gameBridge
        visible: bridge.stage === "playing" && bridge.handPhase === "handComplete"
    }
}
