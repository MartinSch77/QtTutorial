// SPDX-License-Identifier: MIT
import QtQuick

// Two door leaves that visually part when `open` is true and meet in the
// middle when false, instead of a plain "DOORS OPEN"/"DOORS CLOSED" text
// glyph. Drawn as simple rectangles on Canvas.
Canvas {
    id: canvas
    property bool open: false
    property color color: "#f2f4f7"

    property real gapFraction: open ? 0.28 : 0.01

    Behavior on gapFraction { NumberAnimation { duration: 300 } }

    onGapFractionChanged: requestPaint()
    onColorChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()
    Component.onCompleted: requestPaint()

    onPaint: {
        const ctx = getContext("2d");
        ctx.reset();
        const w = width;
        const h = height;
        const halfGap = w * gapFraction * 0.5;
        const centreX = w * 0.5;

        ctx.fillStyle = color;
        // Left leaf.
        ctx.fillRect(w * 0.06, h * 0.08, (centreX - halfGap) - w * 0.06, h * 0.84);
        // Right leaf.
        ctx.fillRect(centreX + halfGap, h * 0.08, (w * 0.94) - (centreX + halfGap), h * 0.84);

        // Frame.
        ctx.strokeStyle = "#243044";
        ctx.lineWidth = Math.max(1, h * 0.03);
        ctx.strokeRect(w * 0.04, h * 0.05, w * 0.92, h * 0.90);
    }
}
