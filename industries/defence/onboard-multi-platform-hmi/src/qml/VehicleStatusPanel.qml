// SPDX-License-Identifier: MIT
import QtQuick

// Simulated crew station status board - stands in for a tank/ship/submarine
// crew display: heading, speed, fuel/power level, and a subsystem health
// grid. Passive readout only; no control inputs are exposed anywhere here.
Rectangle {
    id: root
    property var sim
    color: "#12151b"

    Column {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 24

        Text {
            text: qsTr("Vehicle Crew Display")
            color: "#f2f4f8"
            font.pixelSize: 22
            font.bold: true
        }

        Row {
            spacing: 40

            Column {
                spacing: 4
                Text { text: qsTr("Heading"); color: "#9aa4b2"; font.pixelSize: 14 }
                Text {
                    text: Math.round(root.sim.headingDeg) + "°"
                    color: "#39c0ff"
                    font.pixelSize: 36
                    font.bold: true
                }
            }
            Column {
                spacing: 4
                Text { text: qsTr("Speed"); color: "#9aa4b2"; font.pixelSize: 14 }
                Text {
                    text: root.sim.speedUnits.toFixed(1) + " kn"
                    color: "#39c0ff"
                    font.pixelSize: 36
                    font.bold: true
                }
            }
            Column {
                spacing: 4
                Text { text: qsTr("Fuel / Power"); color: "#9aa4b2"; font.pixelSize: 14 }
                Text {
                    text: root.sim.fuelPercent.toFixed(0) + " %"
                    color: root.sim.fuelPercent < 20 ? "#c0392b" : "#39c0ff"
                    font.pixelSize: 36
                    font.bold: true
                }
            }
        }

        Text {
            text: qsTr("Subsystem Health")
            color: "#9aa4b2"
            font.pixelSize: 14
        }

        HealthGrid {
            subsystems: root.sim.subsystems
        }
    }
}
