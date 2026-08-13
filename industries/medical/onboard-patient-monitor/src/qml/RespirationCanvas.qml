// SPDX-License-Identifier: MIT
import QtQuick

// Draws the rolling respiration buffer as a single polyline, in the
// caution/respiration yellow used across the monitor's colour language.
// Hand-painted with Canvas (no chart library), mirroring EcgCanvas.qml.
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

            const baseY = height * 0.85;
            const scale = height * 0.7;
            const stepX = width / (samples.length - 1);

            ctx.strokeStyle = "#e0c341";
            ctx.lineWidth = 2;
            ctx.beginPath();
            for (let i = 0; i < samples.length; ++i) {
                const x = i * stepX;
                const y = baseY - samples[i] * scale;
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
