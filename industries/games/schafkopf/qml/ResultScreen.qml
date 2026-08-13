// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root
    property var bridge

    readonly property var result: bridge.lastResult

    Column {
        anchors.centerIn: parent
        width: 360
        spacing: 12

        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            color: result.callerTeamWon ? "#3ddc84" : "#e0524a"
            font.pixelSize: 22
            font.bold: true
            text: result.callerTeamWon
                  ? qsTr("Calling team wins!")
                  : qsTr("Calling team loses!")
        }

        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            color: "white"
            text: qsTr("Seat %1 called with seat %2 (%3-Ass) as secret partner")
                    .arg(result.callerSeat).arg(result.partnerSeat).arg(result.calledSuit)
        }

        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            color: "#8fa0b3"
            text: qsTr("Calling team: %1 points  --  Opponents: %2 points")
                    .arg(result.callerTeamPoints).arg(result.opponentTeamPoints)
        }

        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            color: "#e0a94a"
            visible: !!result.schneider || !!result.schwarz
            text: result.schwarz ? qsTr("Schwarz!") : qsTr("Schneider!")
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Deal next hand")
            visible: bridge.isHost
            onClicked: bridge.nextHand()
        }
        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            visible: !bridge.isHost
            color: "#8fa0b3"
            text: qsTr("Waiting for the host to deal the next hand...")
        }
    }
}
