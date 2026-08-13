// SPDX-License-Identifier: MIT
import QtQuick

// A hand-painted warning triangle, drawn on a Canvas (no image/icon-font
// assets) for use anywhere the console needs to flag an out-of-envelope
// reading, e.g. an overloaded payload or an out-of-range travel speed.
Item {
    id: root

    property color triangleColor: "#e5484d"

    onTriangleColorChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const w = width;
            const h = height;

            ctx.fillStyle = root.triangleColor;
            ctx.beginPath();
            ctx.moveTo(w * 0.5, h * 0.06);
            ctx.lineTo(w * 0.95, h * 0.92);
            ctx.lineTo(w * 0.05, h * 0.92);
            ctx.closePath();
            ctx.fill();

            ctx.fillStyle = "#12151b";
            ctx.fillRect(w * 0.46, h * 0.34, w * 0.08, h * 0.28);
            ctx.beginPath();
            ctx.arc(w * 0.5, h * 0.74, w * 0.045, 0, 2 * Math.PI);
            ctx.fill();
        }
    }
}
