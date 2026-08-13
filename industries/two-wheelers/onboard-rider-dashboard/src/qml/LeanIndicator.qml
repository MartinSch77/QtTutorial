// SPDX-License-Identifier: MIT
import QtQuick

// The distinctive two-wheeler HMI element: a horizon-tilt indicator. A fixed
// upright bike silhouette sits in front of a horizon line that rotates by the
// current lean angle, the way a rider's own view of the horizon tilts mid-corner.
// Drawn on a Canvas rather than with pre-rendered art so it stays resolution
// independent.
Item {
    id: root

    property real leanAngleDeg: 0

    onLeanAngleDegChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const cx = width / 2;
            const cy = height / 2;
            const radius = Math.min(width, height) / 2 - 6;

            ctx.save();
            ctx.beginPath();
            ctx.arc(cx, cy, radius, 0, 2 * Math.PI);
            ctx.clip();

            ctx.fillStyle = "#0d1116";
            ctx.fillRect(0, 0, width, height);

            ctx.translate(cx, cy);
            ctx.rotate(root.leanAngleDeg * Math.PI / 180);

            ctx.fillStyle = "#39c0ff";
            ctx.fillRect(-radius * 2, 0, radius * 4, radius * 2);
            ctx.fillStyle = "#12151b";
            ctx.fillRect(-radius * 2, -radius * 2, radius * 4, radius * 2);

            ctx.strokeStyle = "#f2f4f8";
            ctx.lineWidth = 3;
            ctx.beginPath();
            ctx.moveTo(-radius * 2, 0);
            ctx.lineTo(radius * 2, 0);
            ctx.stroke();

            ctx.restore();

            ctx.strokeStyle = "#4a5364";
            ctx.lineWidth = 4;
            ctx.beginPath();
            ctx.arc(cx, cy, radius, 0, 2 * Math.PI);
            ctx.stroke();

            ctx.strokeStyle = "#f2f4f8";
            ctx.lineWidth = 4;
            ctx.beginPath();
            ctx.moveTo(cx - radius * 0.22, cy + radius * 0.5);
            ctx.lineTo(cx, cy - radius * 0.5);
            ctx.lineTo(cx + radius * 0.22, cy + radius * 0.5);
            ctx.stroke();
        }
    }

    Text {
        anchors.top: canvas.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 6
        color: "#f2f4f8"
        font.pixelSize: 16
        font.bold: true
        text: {
            const magnitude = Math.abs(root.leanAngleDeg).toFixed(0);
            if (root.leanAngleDeg > 1) {
                return magnitude + "° R";
            }
            if (root.leanAngleDeg < -1) {
                return magnitude + "° L";
            }
            return "0°";
        }
    }
}
