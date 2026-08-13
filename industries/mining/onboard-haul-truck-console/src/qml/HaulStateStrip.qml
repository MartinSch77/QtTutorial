// SPDX-License-Identifier: MIT
import QtQuick

// Four-segment haul-cycle indicator: the segment matching the truck's current
// phase (Loading / Hauling / Dumping / Returning) is highlighted, giving the
// operator an at-a-glance read of where the truck is in its cycle.
Row {
    id: root

    property int stateIndex: 0
    readonly property var stateLabels: ["LOADING", "HAULING", "DUMPING", "RETURNING"]
    spacing: 8

    Repeater {
        model: root.stateLabels
        Rectangle {
            width: 130
            height: 40
            radius: 6
            color: index === root.stateIndex ? "#39c0ff" : "#1c212b"
            border.color: index === root.stateIndex ? "#39c0ff" : "#2a2f3a"

            Behavior on color {
                ColorAnimation { duration: 150 }
            }

            Text {
                anchors.centerIn: parent
                text: modelData
                color: index === root.stateIndex ? "#0d1116" : "#9aa4b2"
                font.bold: true
                font.pixelSize: 13
            }
        }
    }
}
