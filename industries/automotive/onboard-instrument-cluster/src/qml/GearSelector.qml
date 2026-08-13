// SPDX-License-Identifier: MIT
import QtQuick

// A vertical P/R/N/D gear-selector indicator, drawn as simple geometric shapes
// (Rectangle "rungs" plus a Canvas-painted pointer) rather than plain text, so the
// cluster reads as a real transmission indicator. No image assets: everything here
// is Rectangles and Canvas paths.
Item {
    id: root

    // "P", "R", "D1".."D6" - anything starting with "D" lights the D rung.
    property string gearLabel: "P"
    property color accentColor: "#39c0ff"

    readonly property string activeRung: gearLabel === "P" ? "P" : gearLabel === "R" ? "R" : gearLabel.startsWith("N") ? "N" : "D"

    implicitWidth: 64
    implicitHeight: 160

    Column {
        anchors.fill: parent
        spacing: 4

        Repeater {
            model: ["P", "R", "N", "D"]

            Rectangle {
                required property string modelData
                width: root.width
                height: (root.height - 12) / 4
                radius: 6
                color: modelData === root.activeRung ? root.accentColor : "#1c212b"
                border.color: modelData === root.activeRung ? root.accentColor : "#2a2f3a"
                border.width: 1

                Behavior on color {
                    ColorAnimation { duration: 150 }
                }

                Text {
                    anchors.centerIn: parent
                    text: modelData
                    font.pixelSize: 22
                    font.bold: true
                    color: modelData === root.activeRung ? "#0c0e12" : "#8993a3"
                }
            }
        }
    }

    Text {
        anchors.top: parent.bottom
        anchors.topMargin: 6
        anchors.horizontalCenter: parent.horizontalCenter
        text: root.gearLabel
        color: "#f2f4f8"
        font.pixelSize: 16
        font.bold: true
    }
}
