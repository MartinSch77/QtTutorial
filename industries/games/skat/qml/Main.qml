// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtTutorial.Skat

ApplicationWindow {
    id: window
    width: 960
    height: 680
    visible: true
    title: "Skat (Suit / Grand, LAN multiplayer)"
    color: "#1b2436"

    SkatController {
        id: controller
    }

    readonly property bool declarerIsMe: controller.declarationInfo.declarerSeat === controller.localSeat
    readonly property int sittingOutSeat: controller.tableInfo.sittingOutSeat !== undefined
        ? controller.tableInfo.sittingOutSeat : -1
    readonly property bool spectating: sittingOutSeat === controller.localSeat && controller.phase !== "lobby"

    JoinPanel {
        anchors.centerIn: parent
        visible: !controller.connected
        discoveredGames: controller.discoveredGames
        onHostRequested: (name) => controller.hostGame(name)
        onRefreshRequested: controller.refreshDiscovery()
        onJoinDiscoveredRequested: (index, name) => controller.joinDiscoveredGame(index, name)
        onJoinManualRequested: (host, port, name) => controller.joinManual(host, port, name)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 14
        visible: controller.connected

        RowLayout {
            Layout.fillWidth: true
            Text {
                color: "#dfe6f2"
                text: "You are seat " + controller.localSeat
                    + "  |  Dealer/sitting out: seat " + window.sittingOutSeat
                    + "  |  Phase: " + controller.phase
            }
            Item { Layout.fillWidth: true }
            Text {
                color: "#ffb703"
                visible: controller.statusMessage.length > 0
                text: controller.statusMessage
            }
        }

        Text {
            visible: window.spectating
            color: "#ffd166"
            font.pixelSize: 16
            text: "You are the dealer this hand and sit out -- spectating until the next deal."
        }

        Text {
            visible: controller.declarationInfo.gameType !== undefined
            color: "#a6c8ff"
            text: "Declarer: seat " + controller.declarationInfo.declarerSeat
                + "  Game: " + controller.declarationInfo.gameType
                + (controller.declarationInfo.trumpSuit ? (" (" + controller.declarationInfo.trumpSuit + ")") : "")
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TrickView {
                anchors.centerIn: parent
                trickCards: controller.currentTrick
            }
        }

        Loader {
            Layout.fillWidth: true
            Layout.preferredHeight: item ? item.implicitHeight : 0
            sourceComponent: {
                if (controller.phase === "bidding" && !window.spectating) return biddingComponent
                if (controller.phase === "discarding" && window.declarerIsMe) return discardComponent
                if (controller.phase === "announcing" && window.declarerIsMe) return announceComponent
                if (controller.phase === "handComplete") return resultComponent
                return null
            }
        }

        HandView {
            Layout.alignment: Qt.AlignHCenter
            visible: controller.phase === "playing" && !window.spectating
            cards: controller.myHand
            onCardPlayed: (code) => controller.playCard(code)
        }
    }

    Component {
        id: biddingComponent
        BiddingPanel {
            biddingInfo: controller.biddingInfo
            localSeat: controller.localSeat
            onRaise: (amount) => controller.submitBid(amount)
            onPass: controller.submitPass()
        }
    }

    Component {
        id: discardComponent
        DiscardPanel {
            hand: controller.myHand
            onDiscard: (codes) => controller.submitDiscard(codes)
        }
    }

    Component {
        id: announceComponent
        AnnouncementPanel {
            onAnnounce: (gameType, trumpSuit) => controller.submitAnnouncement(gameType, trumpSuit)
        }
    }

    Component {
        id: resultComponent
        ResultPanel {
            resultInfo: controller.resultInfo
            isHost: controller.isHost
            onNextHand: controller.startNextHand()
        }
    }
}
