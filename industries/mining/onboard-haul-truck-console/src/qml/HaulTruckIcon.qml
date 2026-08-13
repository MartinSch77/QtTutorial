// SPDX-License-Identifier: MIT
import QtQuick

// A simplified, hand-painted silhouette of a rigid mining haul truck (cab,
// raised dump body and two axle groups), drawn on a Canvas as plain
// geometric shapes. This is a generic, non-branded silhouette: no
// manufacturer logo, wordmark or exact vehicle profile is reproduced.
Item {
    id: root

    property color bodyColor: "#9aa4b2"
    property color accentColor: "#39c0ff"

    onBodyColorChanged: canvas.requestPaint()
    onAccentColorChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const w = width;
            const h = height;

            // Dump body (the tilted rear tray).
            ctx.fillStyle = root.accentColor;
            ctx.beginPath();
            ctx.moveTo(w * 0.30, h * 0.28);
            ctx.lineTo(w * 0.92, h * 0.30);
            ctx.lineTo(w * 0.86, h * 0.58);
            ctx.lineTo(w * 0.34, h * 0.58);
            ctx.closePath();
            ctx.fill();

            // Cab.
            ctx.fillStyle = root.bodyColor;
            ctx.beginPath();
            ctx.moveTo(w * 0.06, h * 0.58);
            ctx.lineTo(w * 0.10, h * 0.30);
            ctx.lineTo(w * 0.30, h * 0.30);
            ctx.lineTo(w * 0.34, h * 0.58);
            ctx.closePath();
            ctx.fill();

            // Chassis/underbody.
            ctx.fillStyle = root.bodyColor;
            ctx.fillRect(w * 0.06, h * 0.58, w * 0.86, h * 0.10);

            // Wheels: a front steer wheel and a rear dual-tyre group.
            ctx.fillStyle = "#12151b";
            ctx.beginPath();
            ctx.arc(w * 0.22, h * 0.80, h * 0.15, 0, 2 * Math.PI);
            ctx.fill();
            ctx.beginPath();
            ctx.arc(w * 0.68, h * 0.80, h * 0.17, 0, 2 * Math.PI);
            ctx.fill();
            ctx.beginPath();
            ctx.arc(w * 0.88, h * 0.80, h * 0.17, 0, 2 * Math.PI);
            ctx.fill();

            ctx.fillStyle = "#5c6472";
            [0.22, 0.68, 0.88].forEach(function (fx) {
                ctx.beginPath();
                ctx.arc(w * fx, h * 0.80, h * 0.06, 0, 2 * Math.PI);
                ctx.fill();
            });
        }
    }
}
