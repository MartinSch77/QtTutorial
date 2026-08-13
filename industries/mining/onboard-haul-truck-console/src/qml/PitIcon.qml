// SPDX-License-Identifier: MIT
import QtQuick

// A stylised top-down open-pit glyph: concentric benches stepping down to a
// pit floor, hand-painted on a Canvas. Used as a small site/branding marker
// in the console header - a generic pit shape, not any real mine's plan.
Item {
    id: root

    property color benchColor: "#9aa4b2"
    property color floorColor: "#39c0ff"

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const cx = width / 2;
            const cy = height / 2;
            const maxRadius = Math.min(width, height) / 2 - 1;
            const benchCount = 4;

            for (let i = 0; i < benchCount; ++i) {
                const t = i / benchCount;
                const radius = maxRadius * (1.0 - t * 0.78);
                ctx.strokeStyle = root.benchColor;
                ctx.globalAlpha = 0.35 + 0.5 * t;
                ctx.lineWidth = Math.max(1, maxRadius * 0.12);
                ctx.beginPath();
                ctx.arc(cx, cy, radius, 0, 2 * Math.PI);
                ctx.stroke();
            }

            ctx.globalAlpha = 1.0;
            ctx.fillStyle = root.floorColor;
            ctx.beginPath();
            ctx.arc(cx, cy, maxRadius * 0.18, 0, 2 * Math.PI);
            ctx.fill();
        }
    }
}
