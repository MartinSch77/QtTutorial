// SPDX-License-Identifier: MIT
import QtQuick

// A small procedurally-drawn fuel-gauge glyph: a needle sweeping between "E" and
// "F" over a short arc, echoing the analogue fuel gauges still found on most
// motorcycle dashes even in an otherwise digital cluster.
Item {
    id: root

    property real percent: 100 // 0-100
    property color strokeColor: "#f2f4f8"
    property color accentColor: "#39c0ff"

    readonly property real startAngle: 200 * Math.PI / 180
    readonly property real sweepAngle: 140 * Math.PI / 180
    readonly property real fraction: Math.max(0, Math.min(1, root.percent / 100))

    onFractionChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const cx = width / 2;
            const cy = height * 0.62;
            const radius = Math.min(width, height) * 0.42;

            ctx.lineWidth = Math.max(1.5, radius * 0.16);
            ctx.strokeStyle = "#3a4150";
            ctx.beginPath();
            ctx.arc(cx, cy, radius, root.startAngle, root.startAngle + root.sweepAngle, false);
            ctx.stroke();

            const lowColor = root.fraction < 0.15 ? "#ff5a5a" : root.accentColor;
            ctx.strokeStyle = lowColor;
            ctx.beginPath();
            ctx.arc(cx, cy, radius, root.startAngle, root.startAngle + root.sweepAngle * root.fraction, false);
            ctx.stroke();

            const needleAngle = root.startAngle + root.sweepAngle * root.fraction;
            ctx.strokeStyle = root.strokeColor;
            ctx.lineWidth = Math.max(1.5, radius * 0.1);
            ctx.beginPath();
            ctx.moveTo(cx, cy);
            ctx.lineTo(cx + Math.cos(needleAngle) * radius * 0.85, cy + Math.sin(needleAngle) * radius * 0.85);
            ctx.stroke();

            ctx.fillStyle = root.strokeColor;
            ctx.font = Math.max(8, radius * 0.45) + "px sans-serif";
            ctx.fillText("E", cx - radius * 1.05, cy + radius * 0.55);
            ctx.fillText("F", cx + radius * 0.75, cy + radius * 0.55);
        }
    }

    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()
    Component.onCompleted: canvas.requestPaint()
}
