// SPDX-License-Identifier: MIT
import QtQuick

// The gear-position readout, drawn as a cog/badge outline (a small ring of gear
// teeth) around the current gear digit, rather than a plain rectangle - the kind
// of "gear position indicator" badge found on a sport-bike TFT cluster.
Rectangle {
    id: root

    property string gearLabel: "N"
    property color accentColor: "#39c0ff"

    radius: 8
    color: "#1c212b"

    Canvas {
        id: canvas
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height) * 0.8
        height: width

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const cx = width / 2;
            const cy = height / 2;
            const outerRadius = width / 2 - 2;
            const innerRadius = outerRadius * 0.78;
            const toothCount = 10;

            ctx.fillStyle = root.accentColor;
            ctx.beginPath();
            for (let i = 0; i < toothCount * 2; ++i) {
                const angle = (Math.PI * 2 * i) / (toothCount * 2);
                const r = (i % 2 === 0) ? outerRadius : innerRadius;
                const x = cx + Math.cos(angle) * r;
                const y = cy + Math.sin(angle) * r;
                if (i === 0) {
                    ctx.moveTo(x, y);
                } else {
                    ctx.lineTo(x, y);
                }
            }
            ctx.closePath();
            ctx.globalAlpha = 0.35;
            ctx.fill();
            ctx.globalAlpha = 1.0;

            ctx.strokeStyle = root.accentColor;
            ctx.lineWidth = 2;
            ctx.beginPath();
            ctx.arc(cx, cy, innerRadius * 0.62, 0, 2 * Math.PI);
            ctx.stroke();
        }
    }

    onAccentColorChanged: canvas.requestPaint()
    Component.onCompleted: canvas.requestPaint()

    Text {
        anchors.centerIn: parent
        text: root.gearLabel
        color: "#f2f4f8"
        font.pixelSize: root.height * 0.5
        font.bold: true
    }
}
