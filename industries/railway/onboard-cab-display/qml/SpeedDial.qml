// SPDX-License-Identifier: MIT
import QtQuick

// A circular speed dial in the style of an ETCS/ATP Driver Machine Interface
// speedometer: a 240-degree arc, a needle for current speed, and a target
// marker for the permitted speed — drawn entirely as vector paths on Canvas
// (arcs/lines), not any specific vendor's exact dial artwork.
Canvas {
    id: canvas
    property real speedKmh: 0
    property real permittedSpeedKmh: 0
    property real maxSpeedKmh: 160
    property bool warning: false

    readonly property real startAngle: Math.PI * 0.75   // 135 degrees, pointing lower-left
    readonly property real sweepAngle: Math.PI * 1.5    // 270 degrees total sweep

    function valueToAngle(value) {
        const clamped = Math.max(0, Math.min(value, maxSpeedKmh));
        return startAngle + sweepAngle * (clamped / maxSpeedKmh);
    }

    onSpeedKmhChanged: requestPaint()
    onPermittedSpeedKmhChanged: requestPaint()
    onWarningChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()
    Component.onCompleted: requestPaint()

    onPaint: {
        const ctx = getContext("2d");
        ctx.reset();
        const w = width;
        const h = height;
        const cx = w * 0.5;
        const cy = h * 0.5;
        const radius = Math.min(w, h) * 0.46;

        // Dial face.
        ctx.fillStyle = "#0a0f16";
        ctx.beginPath();
        ctx.arc(cx, cy, radius, 0, Math.PI * 2);
        ctx.fill();

        // Track arc (unlit zone).
        ctx.lineWidth = radius * 0.10;
        ctx.lineCap = "butt";
        ctx.strokeStyle = "#1a222f";
        ctx.beginPath();
        ctx.arc(cx, cy, radius * 0.88, startAngle, startAngle + sweepAngle, false);
        ctx.stroke();

        // Lit zone up to the permitted speed (green), and an over-speed
        // zone beyond it (red) — the same "safe vs. supervised" split a real
        // ATP speedometer shows.
        const permittedAngle = valueToAngle(permittedSpeedKmh);
        ctx.strokeStyle = "#3ddc84";
        ctx.beginPath();
        ctx.arc(cx, cy, radius * 0.88, startAngle, permittedAngle, false);
        ctx.stroke();
        ctx.strokeStyle = "#e5484d";
        ctx.beginPath();
        ctx.arc(cx, cy, radius * 0.88, permittedAngle, startAngle + sweepAngle, false);
        ctx.stroke();

        // Major ticks every 20 km/h with labels.
        ctx.strokeStyle = "#5a6576";
        ctx.fillStyle = "#8a94a3";
        ctx.font = Math.round(radius * 0.12) + "px sans-serif";
        ctx.textAlign = "center";
        ctx.textBaseline = "middle";
        ctx.lineWidth = Math.max(1, radius * 0.02);
        for (let value = 0; value <= maxSpeedKmh; value += 20) {
            const angle = valueToAngle(value);
            const inner = radius * 0.74;
            const outer = radius * 0.82;
            const x1 = cx + Math.cos(angle) * inner;
            const y1 = cy + Math.sin(angle) * inner;
            const x2 = cx + Math.cos(angle) * outer;
            const y2 = cy + Math.sin(angle) * outer;
            ctx.beginPath();
            ctx.moveTo(x1, y1);
            ctx.lineTo(x2, y2);
            ctx.stroke();

            const lx = cx + Math.cos(angle) * radius * 0.60;
            const ly = cy + Math.sin(angle) * radius * 0.60;
            ctx.fillText(String(value), lx, ly);
        }

        // Target-speed marker (permitted speed): a short white tick.
        const targetAngle = valueToAngle(permittedSpeedKmh);
        const tOuter = radius * 0.88;
        const tInner = radius * 0.68;
        ctx.strokeStyle = "#f2f4f7";
        ctx.lineWidth = Math.max(2, radius * 0.03);
        ctx.beginPath();
        ctx.moveTo(cx + Math.cos(targetAngle) * tInner, cy + Math.sin(targetAngle) * tInner);
        ctx.lineTo(cx + Math.cos(targetAngle) * tOuter, cy + Math.sin(targetAngle) * tOuter);
        ctx.stroke();

        // Needle for current speed.
        const needleAngle = valueToAngle(speedKmh);
        const needleLength = radius * 0.66;
        ctx.strokeStyle = warning ? "#e5484d" : "#2f81f7";
        ctx.lineWidth = Math.max(2, radius * 0.045);
        ctx.lineCap = "round";
        ctx.beginPath();
        ctx.moveTo(cx, cy);
        ctx.lineTo(cx + Math.cos(needleAngle) * needleLength, cy + Math.sin(needleAngle) * needleLength);
        ctx.stroke();

        // Hub.
        ctx.fillStyle = "#f2f4f7";
        ctx.beginPath();
        ctx.arc(cx, cy, radius * 0.05, 0, Math.PI * 2);
        ctx.fill();

        // Rim.
        ctx.strokeStyle = "#243044";
        ctx.lineWidth = Math.max(1, radius * 0.02);
        ctx.beginPath();
        ctx.arc(cx, cy, radius, 0, Math.PI * 2);
        ctx.stroke();
    }
}
