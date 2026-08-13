// SPDX-License-Identifier: MIT
import QtQuick

// A hand-painted circular gauge (270 degree sweep), used for both the speedometer
// and the rev counter. No image assets: the arc, ticks and needle are all drawn on
// a Canvas so the dashboard stays fully vector and themeable.
Item {
    id: root

    property real value: 0
    property real minValue: 0
    property real maxValue: 100
    property string label: ""
    property string unit: ""
    property color accentColor: "#39c0ff"
    // Fraction (0-1) of the sweep at which a redline zone begins; 1 (the
    // default) disables it. Used for the rev counter, the way a real
    // sport-bike tacho paints its last ~15-20% of travel red.
    property real redlineStart: 1.0
    property color redlineColor: "#ff3b30"

    readonly property real startAngle: 135 * Math.PI / 180
    readonly property real sweepAngle: 270 * Math.PI / 180
    readonly property real fraction: Math.max(0, Math.min(1, (value - minValue) / (maxValue - minValue)))

    onValueChanged: canvas.requestPaint()
    onRedlineStartChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const cx = width / 2;
            const cy = height / 2;
            const radius = Math.min(width, height) / 2 - 8;

            ctx.lineWidth = radius * 0.14;
            ctx.strokeStyle = "#2a2f3a";
            ctx.beginPath();
            ctx.arc(cx, cy, radius, root.startAngle, root.startAngle + root.sweepAngle, false);
            ctx.stroke();

            if (root.redlineStart < 1.0) {
                ctx.strokeStyle = root.redlineColor;
                ctx.globalAlpha = 0.3;
                ctx.beginPath();
                ctx.arc(cx, cy, radius, root.startAngle + root.sweepAngle * root.redlineStart,
                        root.startAngle + root.sweepAngle, false);
                ctx.stroke();
                ctx.globalAlpha = 1.0;
            }

            const accentEnd = Math.min(root.fraction, root.redlineStart);
            ctx.strokeStyle = root.accentColor;
            ctx.beginPath();
            ctx.arc(cx, cy, radius, root.startAngle, root.startAngle + root.sweepAngle * accentEnd, false);
            ctx.stroke();

            if (root.fraction > root.redlineStart) {
                ctx.strokeStyle = root.redlineColor;
                ctx.beginPath();
                ctx.arc(cx, cy, radius, root.startAngle + root.sweepAngle * root.redlineStart,
                        root.startAngle + root.sweepAngle * root.fraction, false);
                ctx.stroke();
            }

            const needleAngle = root.startAngle + root.sweepAngle * root.fraction;
            ctx.lineWidth = 3;
            ctx.strokeStyle = "#f2f4f8";
            ctx.beginPath();
            ctx.moveTo(cx, cy);
            ctx.lineTo(cx + Math.cos(needleAngle) * (radius - 4), cy + Math.sin(needleAngle) * (radius - 4));
            ctx.stroke();

            ctx.fillStyle = "#f2f4f8";
            ctx.beginPath();
            ctx.arc(cx, cy, 5, 0, 2 * Math.PI);
            ctx.fill();
        }
    }

    Column {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: parent.height * 0.2
        spacing: 2

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: Math.round(root.value) + root.unit
            color: "#f2f4f8"
            font.pixelSize: root.height * 0.14
            font.bold: true
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.label
            color: "#9aa4b2"
            font.pixelSize: root.height * 0.08
        }
    }
}
