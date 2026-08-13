// SPDX-License-Identifier: MIT
import QtQuick

// The default main-viewscreen station: an animated starfield behind an
// LCARS-styled elbow frame, whose bar color follows the ship-wide alert
// level so red alert is visible at a glance even on this panel.
Item {
    id: root
    property var bridge

    readonly property var alertColors: ["#ff9c00", "#e0a300", "#c0392b"]

    Starfield {
        anchors.fill: parent
    }

    LcarsElbowFrame {
        anchors.fill: parent
        barColor: root.bridge ? root.alertColors[root.bridge.alertLevelIndex] : alertColors[0]

        Column {
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            spacing: 4

            Text {
                text: qsTr("HEADING %1°").arg(root.bridge ? root.bridge.headingDeg.toFixed(0) : "0")
                color: "#c6a0ff"
                font.pixelSize: 15
                font.bold: true
            }
            Text {
                text: qsTr("WARP %1  IMPULSE %2%")
                    .arg(root.bridge ? root.bridge.warpFactor.toFixed(1) : "0.0")
                    .arg(root.bridge ? root.bridge.impulsePercent.toFixed(0) : "0")
                color: "#c6a0ff"
                font.pixelSize: 15
                font.bold: true
            }
        }
    }
}
