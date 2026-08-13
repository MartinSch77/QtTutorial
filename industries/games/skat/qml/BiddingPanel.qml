// SPDX-License-Identifier: MIT
import QtQuick
import QtQuick.Controls

// Simplified numeric-raise bidding UI: raise by exactly 1 above the current
// highest bid, or pass. See README "Simplifications" for why this replaces
// the traditional Reizen value-announcement exchange.
Column {
    id: root
    spacing: 10

    property var biddingInfo: ({})
    property int localSeat: -1

    signal raise(int amount)
    signal pass()

    readonly property int highestBid: biddingInfo.highestBid !== undefined ? biddingInfo.highestBid : 0
    readonly property int highestBidder: biddingInfo.highestBidder !== undefined ? biddingInfo.highestBidder : -1
    readonly property int turnSeat: biddingInfo.turnSeat !== undefined ? biddingInfo.turnSeat : -1
    readonly property bool myTurn: turnSeat === localSeat

    Text {
        color: "#f5f5f5"
        font.pixelSize: 16
        text: highestBidder >= 0
            ? "Highest bid: " + highestBid + " (seat " + highestBidder + ")"
            : "No bids yet"
    }

    Text {
        color: "#c8d2e6"
        text: myTurn ? "Your turn to bid" : "Waiting for seat " + turnSeat + "..."
    }

    Row {
        spacing: 10
        visible: root.myTurn
        Button {
            text: "Raise to " + (root.highestBid + 1)
            onClicked: root.raise(root.highestBid + 1)
        }
        Button {
            text: "Pass"
            onClicked: root.pass()
        }
    }
}
