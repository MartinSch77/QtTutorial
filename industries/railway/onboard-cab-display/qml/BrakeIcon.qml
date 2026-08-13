// SPDX-License-Identifier: MIT
import QtQuick

// A brake/warning triangle icon (the general "caution" shape used across
// transport HMIs, not any specific vendor's mark), with a brake-shoe glyph
// inside. Colour intensifies when `active` is true. Drawn on Canvas.
Canvas {
    id: canvas
    property bool active: false
    readonly property color activeColor: "#e5484d"
    readonly property color idleColor: "#3a4452"

    onActiveChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()
    Component.onCompleted: requestPaint()

    onPaint: {
        const ctx = getContext("2d");
        ctx.reset();
        const w = width;
        const h = height;
        const strokeColor = active ? activeColor : idleColor;

        ctx.strokeStyle = strokeColor;
        ctx.lineWidth = Math.max(1, w * 0.08);
        ctx.lineJoin = "round";
        ctx.beginPath();
        ctx.moveTo(w * 0.5, h * 0.08);
        ctx.lineTo(w * 0.94, h * 0.90);
        ctx.lineTo(w * 0.06, h * 0.90);
        ctx.closePath();
        ctx.stroke();

        // Exclamation glyph.
        ctx.fillStyle = strokeColor;
        ctx.fillRect(w * 0.45, h * 0.34, w * 0.10, h * 0.28);
        ctx.beginPath();
        ctx.arc(w * 0.5, h * 0.74, w * 0.06, 0, Math.PI * 2);
        ctx.fill();
    }
}
