// SPDX-License-Identifier: MIT
import QtQuick

// A motor glyph: a circular housing with mounting feet, drawn with Canvas
// paths, coloured by run/stop/fault state (the same three-state palette used
// for the pump elsewhere in this panel) with a small spinning GearIcon riding
// on the shaft to sell "this is turning" at a glance.
Item {
    id: root
    property int stateIndex: 0 // 0 idle, 1 running, 2 fault
    readonly property color motorColor: stateIndex === 1 ? "#3ddc84" : (stateIndex === 2 ? "#e5484d" : "#5a6472")
    implicitWidth: 48
    implicitHeight: 48

    onMotorColorChanged: canvas.requestPaint()
    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const w = width;
            const h = height;

            ctx.fillStyle = "#0d1116";
            ctx.strokeStyle = root.motorColor;
            ctx.lineWidth = 2.5;
            ctx.beginPath();
            ctx.ellipse(1.5, 1.5, w - 3, h - 3);
            ctx.fill();
            ctx.stroke();

            // Mounting feet.
            ctx.beginPath();
            ctx.rect(w * 0.22, h - 3, w * 0.1, 5);
            ctx.rect(w * 0.68, h - 3, w * 0.1, 5);
            ctx.fillStyle = "#0d1116";
            ctx.fill();
            ctx.stroke();
        }
    }

    GearIcon {
        anchors.centerIn: canvas
        width: canvas.width * 0.55
        height: width
        running: root.stateIndex === 1
        tint: root.motorColor
    }
}
