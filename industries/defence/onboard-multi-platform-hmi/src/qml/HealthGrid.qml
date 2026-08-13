// SPDX-License-Identifier: MIT
import QtQuick

// Reads a QVariantList of {name, state} entries (state is "Nominal",
// "Caution" or "Critical", as produced by SubsystemHealthMachine) and draws
// one colored cell per subsystem.
Row {
    id: root
    property var subsystems: []
    spacing: 12

    function colorForState(state) {
        if (state === "Critical") return "#c0392b";
        if (state === "Caution") return "#e0a300";
        return "#2fa84f";
    }

    Repeater {
        model: root.subsystems
        Rectangle {
            width: 140
            height: 80
            radius: 8
            color: colorForState(modelData.state)

            Column {
                anchors.centerIn: parent
                spacing: 4
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: modelData.name
                    color: "white"
                    font.bold: true
                    font.pixelSize: 16
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: modelData.state
                    color: "white"
                    font.pixelSize: 14
                }
            }
        }
    }
}
