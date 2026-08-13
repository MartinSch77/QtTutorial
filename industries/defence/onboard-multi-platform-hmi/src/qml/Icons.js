// SPDX-License-Identifier: MIT
.pragma library

// Small library of procedurally-drawn vector icons shared by the onboard
// panels' QML Canvas instrumentation. Every icon here is built purely from
// geometric primitives (arcs, lines, polygons) - nothing is loaded from an
// icon font or an external image/SVG asset. All of them are purely
// informational/passive: a track glyph shows "there is something here,
// classified as X, heading this way" and nothing about engaging it.

// Draws a generic track/blip glyph shaped by domain (Air/Surface/Ground),
// with a short heading tick - a passive plot marker, never an aim solution.
function drawTrackGlyph(ctx, x, y, domain, headingRad, color, size) {
    ctx.save();
    ctx.fillStyle = color;
    ctx.strokeStyle = color;
    ctx.lineWidth = 2;

    if (domain === "Air") {
        ctx.translate(x, y);
        ctx.rotate(headingRad);
        ctx.beginPath();
        ctx.moveTo(size, 0);
        ctx.lineTo(-size * 0.7, size * 0.6);
        ctx.lineTo(-size * 0.7, -size * 0.6);
        ctx.closePath();
        ctx.fill();
        ctx.restore();
    } else if (domain === "Surface") {
        ctx.beginPath();
        ctx.arc(x, y, size * 0.7, 0, 2 * Math.PI);
        ctx.fill();
        ctx.restore();
    } else {
        ctx.beginPath();
        ctx.rect(x - size * 0.6, y - size * 0.6, size * 1.2, size * 1.2);
        ctx.fill();
        ctx.restore();
    }

    ctx.save();
    ctx.strokeStyle = color;
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(x, y);
    ctx.lineTo(x + Math.cos(headingRad) * size * 2.2, y + Math.sin(headingRad) * size * 2.2);
    ctx.stroke();
    ctx.restore();
}

// Draws a compass-rose overlay (N/E/S/W major ticks, minor ticks every
// 30 degrees) centered at (cx, cy) with the given outer radius.
function drawCompassRose(ctx, cx, cy, radius, color) {
    ctx.save();
    ctx.strokeStyle = color;
    ctx.fillStyle = color;
    ctx.font = "11px sans-serif";
    ctx.lineWidth = 1;
    const labels = ["N", "E", "S", "W"];
    for (let deg = 0; deg < 360; deg += 30) {
        const rad = (deg - 90) * Math.PI / 180;
        const isMajor = deg % 90 === 0;
        const tickLength = isMajor ? 12 : 6;
        const x1 = cx + Math.cos(rad) * radius;
        const y1 = cy + Math.sin(rad) * radius;
        const x2 = cx + Math.cos(rad) * (radius - tickLength);
        const y2 = cy + Math.sin(rad) * (radius - tickLength);
        ctx.beginPath();
        ctx.moveTo(x1, y1);
        ctx.lineTo(x2, y2);
        ctx.stroke();
        if (isMajor) {
            const lx = cx + Math.cos(rad) * (radius + 14);
            const ly = cy + Math.sin(rad) * (radius + 14);
            ctx.fillText(labels[deg / 90], lx - 4, ly + 4);
        }
    }
    ctx.restore();
}

// Draws a filled warning triangle with an exclamation mark - used purely to
// flag a data-quality/health condition (e.g. a stale track, a critical
// subsystem), never a target or threat-priority indicator.
function drawWarningTriangle(ctx, x, y, size, color) {
    ctx.save();
    ctx.fillStyle = color;
    ctx.strokeStyle = "#1a1a1a";
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(x, y - size);
    ctx.lineTo(x - size, y + size * 0.8);
    ctx.lineTo(x + size, y + size * 0.8);
    ctx.closePath();
    ctx.fill();
    ctx.stroke();
    ctx.fillStyle = "#1a1a1a";
    ctx.font = "bold " + Math.round(size * 1.1) + "px sans-serif";
    ctx.textAlign = "center";
    ctx.fillText("!", x, y + size * 0.55);
    ctx.textAlign = "left";
    ctx.restore();
}

// Draws a comms/antenna icon: a mast with a small crossbar and 4 signal
// bars, the number of "lit" bars proportional to a 0-100 link/subsystem
// quality percentage.
function drawAntennaBars(ctx, x, y, size, qualityPercent, color) {
    ctx.save();
    ctx.strokeStyle = color;
    ctx.fillStyle = color;
    ctx.lineWidth = 2;

    ctx.beginPath();
    ctx.moveTo(x, y + size);
    ctx.lineTo(x, y - size * 0.2);
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(x, y - size * 0.2);
    ctx.lineTo(x - size * 0.35, y - size * 0.6);
    ctx.moveTo(x, y - size * 0.2);
    ctx.lineTo(x + size * 0.35, y - size * 0.6);
    ctx.stroke();

    const litBars = Math.max(0, Math.min(4, Math.round(qualityPercent / 25)));
    for (let i = 0; i < 4; ++i) {
        const barHeight = size * 0.25 * (i + 1);
        ctx.globalAlpha = i < litBars ? 1.0 : 0.25;
        ctx.fillRect(x + size * 0.55 + i * size * 0.35, y + size - barHeight, size * 0.25, barHeight);
    }
    ctx.globalAlpha = 1.0;
    ctx.restore();
}

// Draws a shield/status icon: a check mark when `ok` is true, a cross when
// it is false - a generic "is this subsystem/platform healthy" glyph.
function drawShield(ctx, x, y, size, color, ok) {
    ctx.save();
    ctx.fillStyle = color;
    ctx.strokeStyle = "#0c0f14";
    ctx.lineWidth = 1.5;
    ctx.beginPath();
    ctx.moveTo(x, y - size);
    ctx.lineTo(x + size * 0.8, y - size * 0.6);
    ctx.lineTo(x + size * 0.8, y + size * 0.3);
    ctx.quadraticCurveTo(x + size * 0.8, y + size * 1.1, x, y + size * 1.3);
    ctx.quadraticCurveTo(x - size * 0.8, y + size * 1.1, x - size * 0.8, y + size * 0.3);
    ctx.lineTo(x - size * 0.8, y - size * 0.6);
    ctx.closePath();
    ctx.fill();
    ctx.stroke();

    ctx.strokeStyle = "white";
    ctx.lineWidth = 2;
    ctx.beginPath();
    if (ok) {
        ctx.moveTo(x - size * 0.35, y);
        ctx.lineTo(x - size * 0.1, y + size * 0.3);
        ctx.lineTo(x + size * 0.4, y - size * 0.35);
    } else {
        ctx.moveTo(x - size * 0.3, y - size * 0.3);
        ctx.lineTo(x + size * 0.3, y + size * 0.3);
        ctx.moveTo(x + size * 0.3, y - size * 0.3);
        ctx.lineTo(x - size * 0.3, y + size * 0.3);
    }
    ctx.stroke();
    ctx.restore();
}
