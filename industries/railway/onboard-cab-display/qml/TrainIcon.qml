// SPDX-License-Identifier: MIT
import QtQuick

// A simple train-front silhouette, drawn as a geometric vector path on a
// Canvas rather than imported from an icon font or SVG asset. Stylised, not
// a copy of any real manufacturer's rolling-stock design.
Canvas {
    id: canvas
    property color color: "#f2f4f7"

    onColorChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()
    Component.onCompleted: requestPaint()

    onPaint: {
        const ctx = getContext("2d");
        ctx.reset();
        const w = width;
        const h = height;

        ctx.fillStyle = color;
        ctx.strokeStyle = color;
        ctx.lineWidth = Math.max(1, h * 0.03);

        // Cab roof + body: a rounded-front silhouette built from a single path.
        ctx.beginPath();
        ctx.moveTo(w * 0.08, h * 0.95);
        ctx.lineTo(w * 0.08, h * 0.45);
        ctx.bezierCurveTo(w * 0.08, h * 0.15, w * 0.30, h * 0.05, w * 0.5, h * 0.05);
        ctx.bezierCurveTo(w * 0.70, h * 0.05, w * 0.92, h * 0.15, w * 0.92, h * 0.45);
        ctx.lineTo(w * 0.92, h * 0.95);
        ctx.closePath();
        ctx.fill();

        // Windscreen cut-out (drawn in the background colour to read as glass).
        ctx.fillStyle = "#05070a";
        ctx.beginPath();
        ctx.moveTo(w * 0.22, h * 0.42);
        ctx.lineTo(w * 0.30, h * 0.20);
        ctx.lineTo(w * 0.70, h * 0.20);
        ctx.lineTo(w * 0.78, h * 0.42);
        ctx.closePath();
        ctx.fill();

        // Headlight.
        ctx.fillStyle = color;
        ctx.beginPath();
        ctx.arc(w * 0.5, h * 0.72, w * 0.07, 0, Math.PI * 2);
        ctx.fill();

        // Buffer beam.
        ctx.fillRect(w * 0.04, h * 0.90, w * 0.92, h * 0.08);
    }
}
