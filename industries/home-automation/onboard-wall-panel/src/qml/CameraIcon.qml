// SPDX-License-Identifier: MIT
import QtQuick

// A simple security-camera glyph: a rounded body, a lens circle, and a small
// mounting tab - plain geometric paths drawn on a Canvas, no icon font or
// external asset.
Item {
    id: root

    property color strokeColor: "#9aa4b2"
    property bool active: true

    implicitWidth: 24
    implicitHeight: 24

    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const w = width;
            const h = height;
            const color = root.active ? root.strokeColor : "#4a5568";

            ctx.strokeStyle = color;
            ctx.fillStyle = color;
            ctx.lineWidth = 2;
            ctx.lineJoin = "round";

            // Mounting tab.
            ctx.beginPath();
            ctx.moveTo(w * 0.5, h * 0.06);
            ctx.lineTo(w * 0.5, h * 0.2);
            ctx.stroke();

            // Body.
            ctx.beginPath();
            ctx.roundedRect(w * 0.16, h * 0.2, w * 0.68, h * 0.42, 6, 6);
            ctx.stroke();

            // Lens.
            ctx.beginPath();
            ctx.arc(w * 0.5, h * 0.41, w * 0.16, 0, Math.PI * 2);
            ctx.stroke();
            if (root.active) {
                ctx.beginPath();
                ctx.arc(w * 0.5, h * 0.41, w * 0.07, 0, Math.PI * 2);
                ctx.fill();
            }

            // Base plate.
            ctx.beginPath();
            ctx.moveTo(w * 0.3, h * 0.62);
            ctx.lineTo(w * 0.7, h * 0.62);
            ctx.lineTo(w * 0.62, h * 0.74);
            ctx.lineTo(w * 0.38, h * 0.74);
            ctx.closePath();
            ctx.stroke();
        }
    }

    Component.onCompleted: canvas.requestPaint()
    onActiveChanged: canvas.requestPaint()
}
