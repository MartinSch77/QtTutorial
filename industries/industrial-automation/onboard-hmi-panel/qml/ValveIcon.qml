// SPDX-License-Identifier: MIT
import QtQuick

// A P&ID-style gate/globe valve symbol: two triangles meeting at the centre
// (the classic "bowtie" valve glyph) plus a stem and handwheel on top, drawn
// with Canvas paths, not an imported icon. Colour communicates state the same
// way a real HMI would: green/open, amber/closed.
Item {
    id: root
    property bool open: true
    implicitWidth: 40
    implicitHeight: 40

    readonly property color valveColor: open ? "#3ddc84" : "#e5b93d"

    onOpenChanged: canvas.requestPaint()
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
            const bowtieTop = h * 0.42;
            const bowtieHeight = h * 0.5;

            ctx.fillStyle = root.valveColor;
            ctx.strokeStyle = "#0d1116";
            ctx.lineWidth = 1.5;
            ctx.lineJoin = "round";

            // Left triangle.
            ctx.beginPath();
            ctx.moveTo(0, bowtieTop);
            ctx.lineTo(w / 2, bowtieTop + bowtieHeight / 2);
            ctx.lineTo(0, bowtieTop + bowtieHeight);
            ctx.closePath();
            ctx.fill();
            ctx.stroke();

            // Right triangle.
            ctx.beginPath();
            ctx.moveTo(w, bowtieTop);
            ctx.lineTo(w / 2, bowtieTop + bowtieHeight / 2);
            ctx.lineTo(w, bowtieTop + bowtieHeight);
            ctx.closePath();
            ctx.fill();
            ctx.stroke();

            // Stem.
            ctx.strokeStyle = "#5a6472";
            ctx.lineWidth = 4;
            ctx.beginPath();
            ctx.moveTo(w / 2, bowtieTop);
            ctx.lineTo(w / 2, h * 0.12);
            ctx.stroke();

            // Handwheel.
            ctx.strokeStyle = root.valveColor;
            ctx.lineWidth = 3;
            ctx.beginPath();
            ctx.ellipse(w * 0.22, 0, w * 0.56, h * 0.22);
            ctx.stroke();
        }
    }
}
