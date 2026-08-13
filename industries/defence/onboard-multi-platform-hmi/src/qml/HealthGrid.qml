// SPDX-License-Identifier: MIT
import QtQuick

import "Icons.js" as Icons

// Reads a QVariantList of {name, state, value} entries (state is "Nominal",
// "Caution" or "Critical", as produced by SubsystemHealthMachine; value is
// the raw 0-100 simulated reading) and draws one colored cell per subsystem,
// with a small passive status glyph in the corner: a shield for the general
// case, antenna signal bars for the Comms subsystem specifically, and a
// warning triangle overlay whenever a cell is Critical.
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

            Canvas {
                id: cellIcon
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 4
                width: 26
                height: 26
                onPaint: {
                    const ctx = getContext("2d");
                    ctx.reset();
                    if (modelData.name === "Comms") {
                        Icons.drawAntennaBars(ctx, 10, 8, 8, modelData.value !== undefined ? modelData.value : 100,
                                               "white");
                    } else {
                        Icons.drawShield(ctx, 12, 10, 7, "white", modelData.state === "Nominal");
                    }
                    if (modelData.state === "Critical") {
                        Icons.drawWarningTriangle(ctx, 12, 20, 5, "#ffcc00");
                    }
                }
                Timer {
                    interval: 200
                    running: true
                    repeat: true
                    onTriggered: cellIcon.requestPaint()
                }
            }
        }
    }
}
