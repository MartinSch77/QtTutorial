// SPDX-License-Identifier: MIT
import QtQuick

// A simple procedurally-drawn full-face helmet silhouette (dome + visor + chin
// bar). Purely geometric, no manufacturer branding - used as a generic "rider
// safety/status" glyph next to the mode selector.
Item {
    id: root

    property color strokeColor: "#f2f4f8"
    property color fillColor: "transparent"

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const w = width;
            const h = height;
            const cx = w * 0.5;
            const cy = h * 0.48;
            const radius = Math.min(w, h) * 0.42;

            ctx.lineWidth = Math.max(1.5, radius * 0.12);
            ctx.strokeStyle = root.strokeColor;
            ctx.fillStyle = root.fillColor;
            ctx.lineJoin = "round";

            // Dome (top 220 degrees of a circle, closed off at the chin line).
            ctx.beginPath();
            ctx.arc(cx, cy, radius, Math.PI * 0.95, Math.PI * 2.05, false);
            ctx.closePath();
            ctx.fill();
            ctx.stroke();

            // Visor.
            ctx.beginPath();
            ctx.moveTo(cx - radius * 0.85, cy - radius * 0.05);
            ctx.lineTo(cx + radius * 0.9, cy - radius * 0.05);
            ctx.lineTo(cx + radius * 0.7, cy + radius * 0.28);
            ctx.lineTo(cx - radius * 0.55, cy + radius * 0.28);
            ctx.closePath();
            ctx.stroke();

            // Chin vent.
            ctx.beginPath();
            ctx.moveTo(cx - radius * 0.25, cy + radius * 0.72);
            ctx.lineTo(cx + radius * 0.25, cy + radius * 0.72);
            ctx.stroke();
        }
    }

    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()
    Component.onCompleted: canvas.requestPaint()
}
