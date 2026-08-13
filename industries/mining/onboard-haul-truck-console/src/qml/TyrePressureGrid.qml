// SPDX-License-Identifier: MIT
import QtQuick

// Simple 2x3 grid of per-wheel tyre-pressure readouts, standing in for the six
// tyres of a large haul truck (2 front, 4 rear dual). Each cell is colour-coded
// against a nominal pressure so a drifting tyre stands out without needing a
// chart.
Grid {
    id: root

    property var pressuresKPa: [700, 700, 700, 700, 700, 700]
    readonly property var wheelLabels: ["FL", "FR", "RLO", "RLI", "RRO", "RRI"]
    readonly property real nominalKPa: 700

    columns: 2
    rowSpacing: 8
    columnSpacing: 8

    Repeater {
        model: root.wheelLabels.length
        Rectangle {
            width: 90
            height: 50
            radius: 6
            color: "#1c212b"
            border.color: {
                const deviation = Math.abs(root.pressuresKPa[index] - root.nominalKPa);
                if (deviation > 30) return "#e5484d";
                if (deviation > 15) return "#e5b93d";
                return "#3ddc6f";
            }

            Column {
                anchors.centerIn: parent
                spacing: 2

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: root.wheelLabels[index]
                    color: "#9aa4b2"
                    font.pixelSize: 10
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: Math.round(root.pressuresKPa[index]) + " kPa"
                    color: "#f2f4f8"
                    font.pixelSize: 13
                    font.bold: true
                }
            }
        }
    }
}
