// SPDX-License-Identifier: MIT
import QtQuick

// Draws the rolling ECG buffer as a single polyline. Hand-painted with Canvas
// (no chart library) at whatever refresh rate the buffer updates.
Rectangle {
    id: root

    property var samples: []
    color: "#0d1117"
    radius: 8
    border.color: "#2a2f3a"

    onSamplesChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent
        anchors.margins: 10

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const samples = root.samples;
            if (samples.length < 2) {
                return;
            }

            const midY = height / 2;
            const scale = height * 0.4;
            const stepX = width / (samples.length - 1);

            ctx.strokeStyle = "#3ddc6f";
            ctx.lineWidth = 2;
            ctx.beginPath();
            for (let i = 0; i < samples.length; ++i) {
                const x = i * stepX;
                const y = midY - samples[i] * scale;
                if (i === 0) {
                    ctx.moveTo(x, y);
                } else {
                    ctx.lineTo(x, y);
                }
            }
            ctx.stroke();
        }
    }
}
