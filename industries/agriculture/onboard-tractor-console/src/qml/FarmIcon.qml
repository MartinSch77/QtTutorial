// SPDX-License-Identifier: MIT
import QtQuick

// A small set of procedurally-drawn farm-equipment glyphs, hand-painted on a
// Canvas as simple geometric vector paths (no image/SVG/font assets), so the
// console reads as a real farm-equipment console rather than a generic form.
// Supported kinds: "tractor", "field", "seed", "plow", "planter", "fuel".
Item {
    id: root

    property string kind: "tractor"
    property color color: "#3ddc6f"

    onKindChanged: canvas.requestPaint()
    onColorChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const w = width;
            const h = height;
            ctx.strokeStyle = root.color;
            ctx.fillStyle = root.color;
            ctx.lineWidth = Math.max(1.5, w * 0.045);
            ctx.lineJoin = "round";
            ctx.lineCap = "round";

            switch (root.kind) {
            case "tractor":
                drawTractor(ctx, w, h);
                break;
            case "field":
                drawField(ctx, w, h);
                break;
            case "seed":
                drawSeed(ctx, w, h);
                break;
            case "plow":
                drawPlow(ctx, w, h);
                break;
            case "planter":
                drawPlanter(ctx, w, h);
                break;
            case "fuel":
                drawFuel(ctx, w, h);
                break;
            default:
                break;
            }
        }

        function drawTractor(ctx, w, h) {
            // Cab + body silhouette.
            ctx.beginPath();
            ctx.moveTo(w * 0.10, h * 0.62);
            ctx.lineTo(w * 0.10, h * 0.42);
            ctx.lineTo(w * 0.34, h * 0.42);
            ctx.lineTo(w * 0.44, h * 0.20);
            ctx.lineTo(w * 0.62, h * 0.20);
            ctx.lineTo(w * 0.62, h * 0.42);
            ctx.lineTo(w * 0.86, h * 0.42);
            ctx.lineTo(w * 0.86, h * 0.62);
            ctx.closePath();
            ctx.stroke();

            // Rear (large) wheel.
            ctx.beginPath();
            ctx.arc(w * 0.28, h * 0.72, h * 0.20, 0, 2 * Math.PI);
            ctx.stroke();
            // Front (small) wheel.
            ctx.beginPath();
            ctx.arc(w * 0.74, h * 0.76, h * 0.12, 0, 2 * Math.PI);
            ctx.stroke();

            // Exhaust stack.
            ctx.beginPath();
            ctx.moveTo(w * 0.40, h * 0.20);
            ctx.lineTo(w * 0.40, h * 0.06);
            ctx.stroke();
        }

        function drawField(ctx, w, h) {
            ctx.beginPath();
            ctx.rect(w * 0.10, h * 0.15, w * 0.80, h * 0.70);
            ctx.stroke();

            // Parallel crop rows.
            const rows = 4;
            for (let i = 1; i <= rows; ++i) {
                const y = h * 0.15 + (h * 0.70) * (i / (rows + 1));
                ctx.beginPath();
                ctx.moveTo(w * 0.16, y);
                ctx.lineTo(w * 0.84, y);
                ctx.stroke();
            }
        }

        function drawSeed(ctx, w, h) {
            // Leaf: two mirrored bezier curves meeting at tip and base.
            const cx = w * 0.5;
            ctx.beginPath();
            ctx.moveTo(cx, h * 0.85);
            ctx.bezierCurveTo(w * 0.05, h * 0.75, w * 0.10, h * 0.15, cx, h * 0.10);
            ctx.bezierCurveTo(w * 0.90, h * 0.15, w * 0.95, h * 0.75, cx, h * 0.85);
            ctx.closePath();
            ctx.fill();

            // Center vein.
            ctx.strokeStyle = "#12151b";
            ctx.lineWidth = Math.max(1.0, w * 0.02);
            ctx.beginPath();
            ctx.moveTo(cx, h * 0.80);
            ctx.lineTo(cx, h * 0.18);
            ctx.stroke();
        }

        function drawPlow(ctx, w, h) {
            // Tool bar.
            ctx.beginPath();
            ctx.moveTo(w * 0.08, h * 0.30);
            ctx.lineTo(w * 0.92, h * 0.30);
            ctx.stroke();

            // Angled plow blades (shares/discs), drawn as simple triangles.
            const bladeCount = 3;
            for (let i = 0; i < bladeCount; ++i) {
                const x = w * (0.22 + i * 0.28);
                ctx.beginPath();
                ctx.moveTo(x, h * 0.30);
                ctx.lineTo(x + w * 0.14, h * 0.30);
                ctx.lineTo(x + w * 0.05, h * 0.85);
                ctx.closePath();
                ctx.fill();
            }
        }

        function drawPlanter(ctx, w, h) {
            // Tool bar.
            ctx.beginPath();
            ctx.moveTo(w * 0.08, h * 0.28);
            ctx.lineTo(w * 0.92, h * 0.28);
            ctx.stroke();

            // Seed hoppers on top of the bar.
            const hopperCount = 3;
            for (let i = 0; i < hopperCount; ++i) {
                const cx = w * (0.24 + i * 0.26);
                ctx.beginPath();
                ctx.moveTo(cx - w * 0.09, h * 0.28);
                ctx.lineTo(cx + w * 0.09, h * 0.28);
                ctx.lineTo(cx, h * 0.08);
                ctx.closePath();
                ctx.fill();
            }

            // Furrow drop lines.
            for (let i = 0; i < hopperCount; ++i) {
                const cx = w * (0.24 + i * 0.26);
                ctx.beginPath();
                ctx.moveTo(cx, h * 0.28);
                ctx.lineTo(cx, h * 0.88);
                ctx.stroke();
            }
        }

        function drawFuel(ctx, w, h) {
            // Droplet shape.
            const cx = w * 0.5;
            ctx.beginPath();
            ctx.moveTo(cx, h * 0.08);
            ctx.bezierCurveTo(w * 0.90, h * 0.55, w * 0.78, h * 0.92, cx, h * 0.92);
            ctx.bezierCurveTo(w * 0.22, h * 0.92, w * 0.10, h * 0.55, cx, h * 0.08);
            ctx.closePath();
            ctx.fill();
        }
    }
}
