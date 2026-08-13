// SPDX-License-Identifier: MIT
import QtQuick

// A DMI-style "planning area": a horizontal strip of chips, one per upcoming
// speed restriction within the simulator's lookahead window, each showing
// how far away it is and what speed it demands — not just the single next
// restriction the margin panel already shows.
Row {
    id: root
    property var restrictions: []
    spacing: 10

    Repeater {
        model: root.restrictions
        delegate: Rectangle {
            required property var modelData
            width: 118
            height: 56
            radius: 8
            color: "#0d1420"
            border.color: "#243044"

            Row {
                anchors.centerIn: parent
                spacing: 6

                RailIcon {
                    width: 18
                    height: 30
                }

                Column {
                    Text {
                        text: modelData.distanceM.toFixed(0) + " m"
                        color: "#8a94a3"
                        font.pixelSize: 12
                    }
                    Text {
                        text: modelData.speedKmh.toFixed(0) + " km/h"
                        color: "#f2f4f7"
                        font.pixelSize: 16
                        font.bold: true
                    }
                }
            }
        }
    }

    Text {
        visible: root.restrictions.length === 0
        text: "Clear ahead"
        color: "#5a6576"
        font.pixelSize: 13
        height: 56
        verticalAlignment: Text.AlignVCenter
    }
}
