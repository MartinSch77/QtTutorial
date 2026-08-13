// SPDX-License-Identifier: MIT
import QtQuick

// Simplified ADAS-style following-distance indicator: four bars light up from the
// vehicle outward as the gap to traffic ahead increases; a fully-collapsed strip
// (level 0) means the gap has closed to a critical distance.
Row {
    id: root

    property int level: 3
    spacing: 6

    Repeater {
        model: 4
        Rectangle {
            width: 10
            height: 22 + index * 6
            radius: 2
            anchors.bottom: parent.bottom
            color: index < root.level ? "#39c0ff" : "#2a2f3a"

            Behavior on color {
                ColorAnimation { duration: 150 }
            }
        }
    }
}
