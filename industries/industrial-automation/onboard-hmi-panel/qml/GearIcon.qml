// SPDX-License-Identifier: MIT
import QtQuick

// A small procedurally-drawn gear/cog glyph: the outline is a ring of
// trapezoid teeth computed from plain trigonometry at paint time on a
// Canvas, not an imported icon font or SVG asset. Spins continuously while
// `running` is true (mirrors a real cog on a turning shaft) and is tinted
// via `tint`.
Item {
    id: root
    property bool running: false
    property color tint: "#8a94a3"
    property int teeth: 8
    implicitWidth: 32
    implicitHeight: 32

    onTintChanged: canvas.requestPaint()
    onTeethChanged: canvas.requestPaint()
    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        RotationAnimation on rotation {
            running: root.running
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 3000
        }

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const w = width;
            const h = height;
            const cx = w / 2;
            const cy = h / 2;
            const outerR = Math.min(w, h) / 2 - 1;
            const innerR = outerR * 0.62;
            const hubR = outerR * 0.32;
            const toothCount = root.teeth;

            ctx.fillStyle = root.tint;
            ctx.strokeStyle = "#0d1116";
            ctx.lineWidth = 1;
            ctx.beginPath();
            for (let i = 0; i < toothCount; ++i) {
                const a0 = (i / toothCount) * 2 * Math.PI;
                const a1 = a0 + (0.5 / toothCount) * 2 * Math.PI;
                const a2 = a0 + (0.62 / toothCount) * 2 * Math.PI;
                const a3 = a0 + (1.0 / toothCount) * 2 * Math.PI;
                const p0 = [cx + innerR * Math.cos(a0), cy + innerR * Math.sin(a0)];
                const p1 = [cx + outerR * Math.cos(a1), cy + outerR * Math.sin(a1)];
                const p2 = [cx + outerR * Math.cos(a2), cy + outerR * Math.sin(a2)];
                const p3 = [cx + innerR * Math.cos(a3), cy + innerR * Math.sin(a3)];
                if (i === 0) {
                    ctx.moveTo(p0[0], p0[1]);
                } else {
                    ctx.lineTo(p0[0], p0[1]);
                }
                ctx.lineTo(p1[0], p1[1]);
                ctx.lineTo(p2[0], p2[1]);
                ctx.lineTo(p3[0], p3[1]);
            }
            ctx.closePath();
            ctx.fill();
            ctx.stroke();

            // Centre hub bolt.
            ctx.fillStyle = "#0d1116";
            ctx.strokeStyle = root.tint;
            ctx.beginPath();
            ctx.ellipse(cx - hubR, cy - hubR, hubR * 2, hubR * 2);
            ctx.fill();
            ctx.stroke();
        }
    }
}
