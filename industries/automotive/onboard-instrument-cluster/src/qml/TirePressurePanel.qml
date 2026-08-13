// SPDX-License-Identifier: MIT
import QtQuick

// A four-wheel tire-pressure-monitoring mini-display, laid out like the
// plan-view TPMS readouts on modern clusters: a schematic car outline with a
// vector tire icon (Canvas-painted circle + tread marks) at each corner, coloured
// by pressure, plus a warning telltale when any wheel runs low.
Item {
    id: root

    // [frontLeft, frontRight, rearLeft, rearRight] in kPa.
    property var pressures: [220, 220, 220, 220]
    property bool warning: false
    property int lowWheelIndex: -1

    implicitWidth: 220
    implicitHeight: 150

    readonly property var wheelLabels: ["FL", "FR", "RL", "RR"]

    // Schematic vehicle body outline, purely decorative context for the four tiles.
    Rectangle {
        anchors.centerIn: parent
        width: 44
        height: 96
        radius: 14
        color: "transparent"
        border.color: "#2a2f3a"
        border.width: 2
    }

    Grid {
        anchors.fill: parent
        columns: 2
        rowSpacing: 8
        columnSpacing: 60

        Repeater {
            model: 4

            Item {
                id: tile
                required property int index
                width: 70
                height: 68

                readonly property real pressure: root.pressures.length > index ? root.pressures[index] : 220
                readonly property bool low: index === root.lowWheelIndex && root.warning
                readonly property color tileColor: low ? "#ff4b5c" : "#39c0ff"

                Canvas {
                    id: tireCanvas
                    width: 40
                    height: 40
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top

                    Connections {
                        target: tile
                        function onLowChanged() { tireCanvas.requestPaint(); }
                        function onPressureChanged() { tireCanvas.requestPaint(); }
                    }

                    onPaint: {
                        const ctx = getContext("2d");
                        ctx.reset();
                        const cx = width / 2;
                        const cy = height / 2;
                        const r = width / 2 - 3;

                        ctx.strokeStyle = tile.tileColor;
                        ctx.lineWidth = 3;
                        ctx.beginPath();
                        ctx.arc(cx, cy, r, 0, 2 * Math.PI);
                        ctx.stroke();

                        // Tread marks around the tire.
                        ctx.lineWidth = 2;
                        for (let i = 0; i < 8; ++i) {
                            const a = i * Math.PI / 4;
                            ctx.beginPath();
                            ctx.moveTo(cx + Math.cos(a) * (r - 4), cy + Math.sin(a) * (r - 4));
                            ctx.lineTo(cx + Math.cos(a) * (r + 1), cy + Math.sin(a) * (r + 1));
                            ctx.stroke();
                        }
                    }
                }

                Text {
                    anchors.top: tireCanvas.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: root.wheelLabels[tile.index] + " " + Math.round(tile.pressure) + " kPa"
                    color: tile.low ? "#ff4b5c" : "#c7ccd6"
                    font.pixelSize: 11
                    font.bold: tile.low
                }
            }
        }
    }
}
