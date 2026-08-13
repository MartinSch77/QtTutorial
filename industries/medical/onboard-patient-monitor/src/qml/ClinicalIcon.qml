// SPDX-License-Identifier: MIT
import QtQuick

// Small procedurally-drawn clinical icons used as labels next to each
// vital-sign panel, so the UI reads as a real bedside monitor rather than a
// generic dashboard. Every icon is hand-drawn with Canvas paths (arcs,
// lines, bezier curves) at paint time - no external icon font, image, or SVG
// asset is loaded.
Item {
    id: root

    // One of: "heartbeat", "droplet", "cuff", "respiration", "bell".
    property string kind: "heartbeat"
    property color color: "white"

    implicitWidth: 28
    implicitHeight: 28

    onKindChanged: canvas.requestPaint()
    onColorChanged: canvas.requestPaint()
    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            ctx.strokeStyle = root.color;
            ctx.fillStyle = root.color;
            ctx.lineWidth = Math.max(1.5, root.width * 0.07);
            ctx.lineCap = "round";
            ctx.lineJoin = "round";

            const w = root.width;
            const h = root.height;

            switch (root.kind) {
            case "heartbeat": {
                // A simple ECG-style pulse blip: flat, spike up, deep dip, flat.
                ctx.beginPath();
                ctx.moveTo(w * 0.02, h * 0.55);
                ctx.lineTo(w * 0.28, h * 0.55);
                ctx.lineTo(w * 0.40, h * 0.15);
                ctx.lineTo(w * 0.52, h * 0.85);
                ctx.lineTo(w * 0.62, h * 0.55);
                ctx.lineTo(w * 0.80, h * 0.55);
                ctx.lineTo(w * 0.86, h * 0.42);
                ctx.lineTo(w * 0.98, h * 0.42);
                ctx.stroke();
                break;
            }
            case "droplet": {
                // A classic teardrop: a point at the top, widening into a
                // rounded bottom via two symmetric bezier curves.
                ctx.beginPath();
                ctx.moveTo(w * 0.5, h * 0.06);
                ctx.bezierCurveTo(w * 0.85, h * 0.45, w * 0.82, h * 0.72, w * 0.5, h * 0.94);
                ctx.bezierCurveTo(w * 0.18, h * 0.72, w * 0.15, h * 0.45, w * 0.5, h * 0.06);
                ctx.closePath();
                ctx.fill();
                break;
            }
            case "cuff": {
                // A blood-pressure cuff: rounded body, a bulb, and a tube.
                ctx.beginPath();
                ctx.roundedRect(w * 0.08, h * 0.28, w * 0.5, h * 0.44, w * 0.08, h * 0.08);
                ctx.stroke();
                // strap buckle line
                ctx.beginPath();
                ctx.moveTo(w * 0.33, h * 0.28);
                ctx.lineTo(w * 0.33, h * 0.72);
                ctx.stroke();
                // tube from cuff to bulb
                ctx.beginPath();
                ctx.moveTo(w * 0.58, h * 0.5);
                ctx.lineTo(w * 0.76, h * 0.5);
                ctx.stroke();
                // bulb
                ctx.beginPath();
                ctx.ellipse(w * 0.76, h * 0.34, w * 0.18, h * 0.32);
                ctx.fill();
                break;
            }
            case "respiration": {
                // A smooth two-hump breathing wave.
                ctx.beginPath();
                ctx.moveTo(w * 0.02, h * 0.6);
                ctx.bezierCurveTo(w * 0.15, h * 0.1, w * 0.30, h * 0.1, w * 0.45, h * 0.6);
                ctx.bezierCurveTo(w * 0.58, h * 1.0, w * 0.68, h * 1.0, w * 0.78, h * 0.55);
                ctx.bezierCurveTo(w * 0.85, h * 0.25, w * 0.92, h * 0.25, w * 0.98, h * 0.45);
                ctx.stroke();
                break;
            }
            case "bell": {
                // An alarm bell: an arc body, a base rail, and a clapper.
                ctx.beginPath();
                ctx.arc(w * 0.5, h * 0.42, w * 0.32, Math.PI, 0, false);
                ctx.lineTo(w * 0.82, h * 0.68);
                ctx.lineTo(w * 0.18, h * 0.68);
                ctx.closePath();
                ctx.fill();
                ctx.beginPath();
                ctx.moveTo(w * 0.12, h * 0.72);
                ctx.lineTo(w * 0.88, h * 0.72);
                ctx.stroke();
                ctx.beginPath();
                ctx.ellipse(w * 0.42, h * 0.76, w * 0.16, h * 0.14);
                ctx.fill();
                break;
            }
            }
        }
    }
}
