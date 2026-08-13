// SPDX-License-Identifier: MIT
import QtQuick

// Riding-mode selector: Rain/Road/Sport/Race, the genre of control found on
// modern sport-bike TFT dashes (a rider-selectable power-delivery map). Tapping
// a mode calls into RiderTelemetry.setRidingMode(), which forwards to
// RideCycleSimulator so the mode visibly changes the dash's accent colour *and*
// the simulated power-delivery curve/lean limit/fuel burn - not just a label.
Rectangle {
    id: root

    property int currentModeIndex: 1 // Road
    property var modeNames: ["RAIN", "ROAD", "SPORT", "RACE"]
    property var modeColors: ["#39c0ff", "#5ad46a", "#ff9f43", "#ff4d4d"]

    signal modeSelected(int modeIndex)

    radius: 8
    color: "#1c212b"

    readonly property color accentColor: modeColors[Math.max(0, Math.min(3, currentModeIndex))]

    Row {
        anchors.centerIn: parent
        spacing: 6

        Repeater {
            model: root.modeNames.length

            Rectangle {
                readonly property bool selected: index === root.currentModeIndex

                width: 56
                height: root.height - 16
                radius: 6
                color: selected ? root.modeColors[index] : "#262c38"
                border.width: selected ? 0 : 1
                border.color: "#3a4150"

                Text {
                    anchors.centerIn: parent
                    text: root.modeNames[index]
                    color: selected ? "#12151b" : "#9aa4b2"
                    font.pixelSize: 11
                    font.bold: true
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.modeSelected(index)
                }
            }
        }
    }
}
