// SPDX-License-Identifier: MIT
import QtQuick

// A simple procedurally-drawn side-view motorcycle silhouette: two wheels, a
// seat/tank line and a headlight dot. Deliberately generic/geometric - it is a
// genre icon (the kind of glyph you would find in any sport-bike dash), not a
// reproduction of any manufacturer's logo or wordmark.
Item {
    id: root

    property color strokeColor: "#f2f4f8"

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const w = width;
            const h = height;
            const wheelRadius = h * 0.22;
            const rearX = w * 0.24;
            const frontX = w * 0.76;
            const wheelY = h * 0.78;

            ctx.strokeStyle = root.strokeColor;
            ctx.lineWidth = Math.max(1.5, h * 0.05);
            ctx.lineJoin = "round";
            ctx.lineCap = "round";

            // Wheels.
            ctx.beginPath();
            ctx.arc(rearX, wheelY, wheelRadius, 0, 2 * Math.PI);
            ctx.stroke();
            ctx.beginPath();
            ctx.arc(frontX, wheelY, wheelRadius, 0, 2 * Math.PI);
            ctx.stroke();

            // Body: rear wheel -> seat -> tank -> steering head -> front wheel.
            ctx.beginPath();
            ctx.moveTo(rearX, wheelY - wheelRadius * 0.3);
            ctx.lineTo(w * 0.40, h * 0.42);
            ctx.lineTo(w * 0.58, h * 0.40);
            ctx.lineTo(w * 0.66, h * 0.52);
            ctx.lineTo(frontX, wheelY - wheelRadius * 0.3);
            ctx.stroke();

            // Handlebar.
            ctx.beginPath();
            ctx.moveTo(w * 0.66, h * 0.52);
            ctx.lineTo(w * 0.78, h * 0.30);
            ctx.stroke();

            // Headlight.
            ctx.fillStyle = root.strokeColor;
            ctx.beginPath();
            ctx.arc(frontX + wheelRadius * 0.1, h * 0.44, h * 0.05, 0, 2 * Math.PI);
            ctx.fill();
        }
    }

    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()
    Component.onCompleted: canvas.requestPaint()
}
