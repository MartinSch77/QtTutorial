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

    LobbyScreen {
        anchors.fill: parent
        bridge: bridge
        visible: bridge.stage !== "playing"
    }

    BiddingScreen {
        anchors.fill: parent
        bridge: bridge
        visible: bridge.stage === "playing" && bridge.handPhase === "bidding"
    }

    PlayingScreen {
        anchors.fill: parent
        bridge: bridge
        visible: bridge.stage === "playing" && bridge.handPhase === "playing"
    }

    ResultScreen {
        anchors.fill: parent
        bridge: bridge
        visible: bridge.stage === "playing" && bridge.handPhase === "handComplete"
    }
}
