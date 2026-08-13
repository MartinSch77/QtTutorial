// SPDX-License-Identifier: MIT
import QtQuick

// A stylised weigh-scale/payload glyph (a suspended weight below a beam),
// hand-painted on a Canvas, used to label payload readouts.
Item {
    id: root

    property color glyphColor: "#3ddc6f"

    onGlyphColorChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const w = width;
            const h = height;

            ctx.strokeStyle = root.glyphColor;
            ctx.fillStyle = root.glyphColor;
            ctx.lineWidth = Math.max(1.5, w * 0.05);

            // Beam.
            ctx.beginPath();
            ctx.moveTo(w * 0.15, h * 0.18);
            ctx.lineTo(w * 0.85, h * 0.18);
            ctx.stroke();

            // Hanger.
            ctx.beginPath();
            ctx.moveTo(w * 0.5, h * 0.18);
            ctx.lineTo(w * 0.5, h * 0.34);
            ctx.stroke();

            // Suspended weight (trapezoid) representing the payload mass.
            ctx.beginPath();
            ctx.moveTo(w * 0.28, h * 0.36);
            ctx.lineTo(w * 0.72, h * 0.36);
            ctx.lineTo(w * 0.86, h * 0.90);
            ctx.lineTo(w * 0.14, h * 0.90);
            ctx.closePath();
            ctx.fill();
        }
    }
}
