// SPDX-License-Identifier: MIT
import QtQuick

// A stylised top-down track icon (two rails plus sleepers) used wherever the
// UI refers to the route/track rather than the train itself. Drawn as plain
// geometric strokes on Canvas.
Canvas {
    id: canvas
    property color color: "#8a94a3"

    onColorChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()
    Component.onCompleted: requestPaint()

    onPaint: {
        const ctx = getContext("2d");
        ctx.reset();
        const w = width;
        const h = height;

        ctx.strokeStyle = color;
        ctx.lineCap = "round";

        // Sleepers.
        ctx.lineWidth = Math.max(1, h * 0.10);
        const sleeperCount = 4;
        for (let i = 0; i < sleeperCount; ++i) {
            const y = h * (0.15 + i * 0.23);
            ctx.beginPath();
            ctx.moveTo(w * 0.05, y);
            ctx.lineTo(w * 0.95, y);
            ctx.stroke();
        }

        // Rails.
        ctx.lineWidth = Math.max(1, h * 0.08);
        ctx.beginPath();
        ctx.moveTo(w * 0.22, 0);
        ctx.lineTo(w * 0.22, h);
        ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(w * 0.78, 0);
        ctx.lineTo(w * 0.78, h);
        ctx.stroke();
    }
}
