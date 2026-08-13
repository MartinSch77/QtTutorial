// SPDX-License-Identifier: MIT
import QtQuick

// 2x3 grid of per-wheel tyre status readouts, standing in for the six tyres
// of a large haul truck (2 front steer, 4 rear dual). Each cell shows a
// hand-painted TyreIcon whose fill sweeps from cool to hot with the tyre's
// temperature, plus the pressure reading colour-coded against a nominal
// pressure so a drifting tyre stands out without needing a chart.
Grid {
    id: root

    property var pressuresKPa: [700, 700, 700, 700, 700, 700]
    property var tempsC: [55, 55, 55, 55, 55, 55]
    readonly property var wheelLabels: ["FL", "FR", "RLO", "RLI", "RRO", "RRI"]
    readonly property real nominalKPa: 700

    columns: 2
    rowSpacing: 8
    columnSpacing: 8

    Repeater {
        model: root.wheelLabels.length
        Rectangle {
            width: 104
            height: 66
            radius: 6
            color: "#1c212b"
            border.color: {
                const deviation = Math.abs(root.pressuresKPa[index] - root.nominalKPa);
                if (deviation > 30) return "#e5484d";
                if (deviation > 15) return "#e5b93d";
                return "#3ddc6f";
            }

            Row {
                anchors.centerIn: parent
                spacing: 8

                TyreIcon {
                    width: 40
                    height: 40
                    anchors.verticalCenter: parent.verticalCenter
                    temperatureC: root.tempsC[index]
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 2

                    Text {
                        text: root.wheelLabels[index]
                        color: "#9aa4b2"
                        font.pixelSize: 10
                    }
                    Text {
                        text: Math.round(root.pressuresKPa[index]) + " kPa"
                        color: "#f2f4f8"
                        font.pixelSize: 12
                        font.bold: true
                    }
                    Text {
                        text: Math.round(root.tempsC[index]) + " °C"
                        color: "#9aa4b2"
                        font.pixelSize: 11
                    }
                }
            }
        }
    }
}
