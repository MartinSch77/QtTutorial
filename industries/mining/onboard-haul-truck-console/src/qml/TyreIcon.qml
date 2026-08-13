// SPDX-License-Identifier: MIT
import QtQuick

// A hand-painted tyre glyph (outer tread ring + tread lugs + hub) whose fill
// colour sweeps from cool blue-green to hot red as temperatureC rises, so a
// heat-soaked tyre reads as an at-a-glance colour rather than a number.
Item {
    id: root

    property real temperatureC: 60
    property real coolTemperatureC: 55
    property real hotTemperatureC: 95

    readonly property real heatFraction: Math.max(0, Math.min(1,
        (temperatureC - coolTemperatureC) / Math.max(1, hotTemperatureC - coolTemperatureC)))

    onHeatFractionChanged: canvas.requestPaint()

    function heatColor(fraction) {
        // Cool = teal/green, hot = amber/red - a simple two-stop gradient
        // computed by hand rather than pulled from an external palette.
        const cool = { r: 0x3d, g: 0xdc, b: 0x6f };
        const hot = { r: 0xe5, g: 0x48, b: 0x4d };
        const r = Math.round(cool.r + (hot.r - cool.r) * fraction);
        const g = Math.round(cool.g + (hot.g - cool.g) * fraction);
        const b = Math.round(cool.b + (hot.b - cool.b) * fraction);
        return "rgb(" + r + "," + g + "," + b + ")";
    }

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const cx = width / 2;
            const cy = height / 2;
            const outerRadius = Math.min(width, height) / 2 - 2;

            ctx.fillStyle = "#12151b";
            ctx.beginPath();
            ctx.arc(cx, cy, outerRadius, 0, 2 * Math.PI);
            ctx.fill();

            ctx.strokeStyle = root.heatColor(root.heatFraction);
            ctx.lineWidth = outerRadius * 0.22;
            ctx.beginPath();
            ctx.arc(cx, cy, outerRadius * 0.78, 0, 2 * Math.PI);
            ctx.stroke();

            // Tread lugs.
            ctx.fillStyle = "#12151b";
            for (let i = 0; i < 8; ++i) {
                const angle = (i / 8) * 2 * Math.PI;
                const lx = cx + Math.cos(angle) * outerRadius * 0.78;
                const ly = cy + Math.sin(angle) * outerRadius * 0.78;
                ctx.beginPath();
                ctx.arc(lx, ly, outerRadius * 0.07, 0, 2 * Math.PI);
                ctx.fill();
            }

            // Hub.
            ctx.fillStyle = "#5c6472";
            ctx.beginPath();
            ctx.arc(cx, cy, outerRadius * 0.32, 0, 2 * Math.PI);
            ctx.fill();
        }
    }
}
