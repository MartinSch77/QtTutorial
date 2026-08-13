// SPDX-License-Identifier: MIT
import QtQuick

// Eco/Comfort/Sport driving-mode selector, the same genre of control found on
// modern digital clusters (a segmented pill that also re-tints the ambient accent
// colour of the cluster once selected). Purely presentational; the actual mode
// switch and its effect on the simulated drivetrain lives in VehicleTelemetry /
// DriveCycleSimulator.
Row {
    id: root

    property int currentMode: 1 // 0 = Eco, 1 = Comfort, 2 = Sport
    property color accentColor: "#39c0ff"
    signal modeSelected(int mode)

    spacing: 6

    readonly property var modeNames: [qsTr("ECO"), qsTr("COMFORT"), qsTr("SPORT")]
    readonly property var modeColors: ["#3ddc6f", "#39c0ff", "#ff4b5c"]

    Repeater {
        model: 3

        Rectangle {
            required property int index
            width: 74
            height: 34
            radius: 8
            color: index === root.currentMode ? root.modeColors[index] : "#1c212b"
            border.color: index === root.currentMode ? root.modeColors[index] : "#2a2f3a"
            border.width: 1

            Behavior on color { ColorAnimation { duration: 150 } }

            Text {
                anchors.centerIn: parent
                text: root.modeNames[index]
                font.pixelSize: 12
                font.bold: true
                color: index === root.currentMode ? "#0c0e12" : "#9aa4b2"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: root.modeSelected(index)
            }
        }
    }
}
