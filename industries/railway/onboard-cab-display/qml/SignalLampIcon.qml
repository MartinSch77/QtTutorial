// SPDX-License-Identifier: MIT
import QtQuick

// A three-lamp signal head icon: a dark housing with red/yellow/green lamps,
// the active one lit with a glow and the others dimmed. Geometric vector
// shapes drawn on Canvas — no image asset, no real signalling-manufacturer
// housing shape reproduced.
Canvas {
    id: canvas
    // 0 = green, 1 = yellow, 2 = red — matches TrainSimulator::SignalAspect.
    property int aspectIndex: 0
    readonly property var litColors: ["#3ddc84", "#e5b93d", "#e5484d"]

    onAspectIndexChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()
    Component.onCompleted: requestPaint()

    onPaint: {
        const ctx = getContext("2d");
        ctx.reset();
        const w = width;
        const h = height;

        // Housing.
        ctx.fillStyle = "#0a0f16";
        ctx.strokeStyle = "#243044";
        ctx.lineWidth = Math.max(1, w * 0.05);
        const radius = w * 0.12;
        ctx.beginPath();
        ctx.moveTo(w * 0.1 + radius, h * 0.05);
        ctx.arcTo(w * 0.9, h * 0.05, w * 0.9, h * 0.95, radius);
        ctx.arcTo(w * 0.9, h * 0.95, w * 0.1, h * 0.95, radius);
        ctx.arcTo(w * 0.1, h * 0.95, w * 0.1, h * 0.05, radius);
        ctx.arcTo(w * 0.1, h * 0.05, w * 0.9, h * 0.05, radius);
        ctx.closePath();
        ctx.fill();
        ctx.stroke();

        // Lamps: red (top), yellow (middle), green (bottom) — the classic
        // vertical stacking order, independent of which one is lit.
        const lampColors = ["#e5484d", "#e5b93d", "#3ddc84"];
        // aspectIndex 0=green(bottom lamp), 1=yellow(middle), 2=red(top).
        const litLampRow = aspectIndex === 0 ? 2 : (aspectIndex === 1 ? 1 : 0);
        const centreX = w * 0.5;
        const lampRadius = w * 0.16;
        for (let row = 0; row < 3; ++row) {
            const centreY = h * (0.25 + row * 0.25) + h * 0.05;
            const isLit = row === litLampRow;
            if (isLit) {
                const glow = ctx.createRadialGradient(centreX, centreY, 0, centreX, centreY, lampRadius * 2.2);
                glow.addColorStop(0, lampColors[row]);
                glow.addColorStop(1, "transparent");
                ctx.fillStyle = glow;
                ctx.beginPath();
                ctx.arc(centreX, centreY, lampRadius * 2.2, 0, Math.PI * 2);
                ctx.fill();
            }
            ctx.fillStyle = isLit ? lampColors[row] : "#2a3342";
            ctx.beginPath();
            ctx.arc(centreX, centreY, lampRadius, 0, Math.PI * 2);
            ctx.fill();
        }
    }
}
