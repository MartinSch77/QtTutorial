// SPDX-License-Identifier: MIT
import QtQuick

// A sun (rayed circle) or crescent moon glyph for the Morning/Night scene
// cards - plain geometric paths drawn on a Canvas, no icon font or external
// asset.
Item {
    id: root

    property bool sun: true
    property color color: sun ? "#f4c860" : "#9aa4b2"

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
            const cx = w / 2;
            const cy = h / 2;

            ctx.strokeStyle = root.color;
            ctx.fillStyle = root.color;
            ctx.lineWidth = 2;
            ctx.lineCap = "round";

            if (root.sun) {
                const r = w * 0.22;
                ctx.beginPath();
                ctx.arc(cx, cy, r, 0, Math.PI * 2);
                ctx.fill();

                for (let i = 0; i < 8; ++i) {
                    const angle = (Math.PI * 2 * i) / 8;
                    const x1 = cx + Math.cos(angle) * r * 1.4;
                    const y1 = cy + Math.sin(angle) * r * 1.4;
                    const x2 = cx + Math.cos(angle) * r * 2.0;
                    const y2 = cy + Math.sin(angle) * r * 2.0;
                    ctx.beginPath();
                    ctx.moveTo(x1, y1);
                    ctx.lineTo(x2, y2);
                    ctx.stroke();
                }
            } else {
                const r = w * 0.32;
                ctx.beginPath();
                ctx.arc(cx, cy, r, 0, Math.PI * 2);
                ctx.fill();
                ctx.globalCompositeOperation = "destination-out";
                ctx.beginPath();
                ctx.arc(cx + r * 0.55, cy - r * 0.35, r * 0.85, 0, Math.PI * 2);
                ctx.fill();
                ctx.globalCompositeOperation = "source-over";
            }
        }
    }

    Component.onCompleted: canvas.requestPaint()
    onSunChanged: canvas.requestPaint()
}
