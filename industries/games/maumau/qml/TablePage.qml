// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls

Item {
    id: root
    property var controller

    readonly property bool waitingRoom: controller.phase === "lobby"
    readonly property bool myTurn: !waitingRoom && controller.currentSeat === controller.mySeat
        && controller.phase === "playing"

    Component {
        id: flyingCardComponent
        PlayingCard {
            parent: root
            z: 100
        }
    }

    function flyCard(fromItem, toX, toY, faceUp, rank, suit) {
        const startPos = fromItem.mapToItem(root, 0, 0);
        const clone = flyingCardComponent.createObject(root, {
            x: startPos.x, y: startPos.y, rank: rank, suit: suit, faceUp: faceUp
        });
        const anim = Qt.createQmlObject(
            "import QtQuick; ParallelAnimation { running: true }", clone);
        const moveX = Qt.createQmlObject(
            "import QtQuick; NumberAnimation { duration: 380; easing.type: Easing.InOutQuad }", anim);
        moveX.target = clone;
        moveX.property = "x";
        moveX.to = toX;
        const moveY = Qt.createQmlObject(
            "import QtQuick; NumberAnimation { duration: 380; easing.type: Easing.InOutQuad }", anim);
        moveY.target = clone;
        moveY.property = "y";
        moveY.to = toY;
        anim.addAnimation(moveX);
        anim.addAnimation(moveY);
        anim.finished.connect(function () { clone.destroy(); });
        anim.restart();
    }

    Rectangle {
        anchors.fill: parent
        color: "#0f2a1c"
    }

    Column {
        visible: root.waitingRoom
        anchors.centerIn: parent
        spacing: 16
        Text {
            text: qsTr("Waiting room — seat %1 of %2").arg(root.controller.mySeat).arg(root.controller.seatCount)
            color: "white"
            font.pixelSize: 20
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Text {
            text: qsTr("Any seat still empty when you start will be filled by a basic bot.")
            color: "#8fa0b3"
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Button {
            visible: root.controller.isHost
            text: qsTr("Start Game")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: root.controller.startRound()
        }
    }

    Item {
        visible: !root.waitingRoom
        anchors.fill: parent

        Row {
            id: opponentsRow
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 16
            spacing: 24
            Repeater {
                model: root.controller.handCounts
                delegate: Column {
                    visible: index !== root.controller.mySeat
                    spacing: 4
                    Text {
                        text: qsTr("Seat %1%2").arg(index).arg(index === root.controller.currentSeat ? " ►" : "")
                        color: index === root.controller.currentSeat ? "#e0a94a" : "white"
                        font.bold: index === root.controller.currentSeat
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Text {
                        text: qsTr("%1 cards").arg(modelData)
                        color: "#8fa0b3"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }
        }

        Row {
            id: pilesRow
            anchors.centerIn: parent
            spacing: 40

            Column {
                spacing: 6
                Text { text: qsTr("Draw pile (%1)").arg(root.controller.drawPileCount); color: "#8fa0b3" }
                PlayingCard {
                    id: drawPileCard
                    faceUp: false
                    MouseArea {
                        anchors.fill: parent
                        enabled: root.myTurn
                        onClicked: {
                            root.flyCard(drawPileCard, handRow.x + handRow.width, handRow.y, false, "", "");
                            root.controller.drawCard();
                        }
                    }
                }
            }

            Column {
                spacing: 6
                Text {
                    text: root.controller.wishedSuit.length > 0
                        ? qsTr("Wished: %1").arg(root.controller.wishedSuit) : qsTr("Discard pile")
                    color: "#8fa0b3"
                }
                PlayingCard {
                    id: discardCard
                    rank: root.controller.topCardRank.length > 0 ? root.controller.topCardRank : "Seven"
                    suit: root.controller.topCardSuit.length > 0 ? root.controller.topCardSuit : "Clubs"
                    faceUp: true
                }
            }
        }

        Rectangle {
            id: noticeBanner
            anchors.top: pilesRow.bottom
            anchors.topMargin: 12
            anchors.horizontalCenter: parent.horizontalCenter
            width: noticeText.implicitWidth + 32
            height: 36
            radius: 18
            color: "#e0a94a"
            opacity: 0
            Behavior on opacity { NumberAnimation { duration: 300 } }
            Text {
                id: noticeText
                anchors.centerIn: parent
                text: root.controller.lastNotice
                color: "#1c232c"
                font.bold: true
            }
        }

        Connections {
            target: root.controller
            function onLastNoticeChanged() {
                noticeBanner.opacity = 1
                noticeTimer.restart()
            }
        }

        Timer {
            id: noticeTimer
            interval: 2200
            onTriggered: noticeBanner.opacity = 0
        }

        Text {
            visible: root.controller.phase === "round_over"
            anchors.centerIn: parent
            text: root.controller.winnerSeat === root.controller.mySeat
                ? qsTr("You win!") : qsTr("Seat %1 wins!").arg(root.controller.winnerSeat)
            color: "#e0a94a"
            font.pixelSize: 36
            font.bold: true
        }

        Text {
            visible: root.myTurn
            anchors.bottom: handRow.top
            anchors.bottomMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Your turn")
            color: "#e0a94a"
            font.pixelSize: 16
            font.bold: true
        }

        Row {
            id: handRow
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 24
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: -20
            move: Transition {
                NumberAnimation { properties: "x,y"; duration: 220; easing.type: Easing.OutQuad }
            }
            Repeater {
                model: root.controller.myHand
                delegate: PlayingCard {
                    id: handCard
                    rank: modelData.rank
                    suit: modelData.suit
                    legal: root.myTurn && root.controller.isCardLegal(modelData.rank, modelData.suit)
                    highlighted: false

                    MouseArea {
                        anchors.fill: parent
                        enabled: handCard.legal
                        onClicked: {
                            if (modelData.rank === "Jack") {
                                suitPicker.pendingRank = modelData.rank;
                                suitPicker.pendingSuit = modelData.suit;
                                suitPicker.pendingItem = handCard;
                                suitPicker.open();
                            } else {
                                root.flyCard(handCard, discardCard.mapToItem(root, 0, 0).x,
                                             discardCard.mapToItem(root, 0, 0).y, true, modelData.rank, modelData.suit);
                                root.controller.playCard(modelData.rank, modelData.suit, "");
                            }
                        }
                    }
                }
            }
        }
    }

    SuitPickerPopup {
        id: suitPicker
        anchors.centerIn: parent
        property string pendingRank: ""
        property string pendingSuit: ""
        property var pendingItem: null
        onSuitChosen: function (suit) {
            if (pendingItem) {
                const target = discardCard.mapToItem(root, 0, 0);
                root.flyCard(pendingItem, target.x, target.y, true, pendingRank, pendingSuit);
            }
            root.controller.playCard(pendingRank, pendingSuit, suit);
            close();
        }
    }
}
