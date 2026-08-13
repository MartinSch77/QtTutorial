// SPDX-License-Identifier: MIT
import QtQuick

// A hand-painted circular gauge (270 degree sweep), used for the payload,
// engine-temperature and retarder-temperature readouts. No image assets: the
// arc, ticks and needle are all drawn on a Canvas so the console stays fully
// vector and themeable.
Item {
    id: root

    property real value: 0
    property real minValue: 0
    property real maxValue: 100
    property string label: ""
    property string unit: ""
    property color accentColor: "#39c0ff"
    property bool alarm: false

    // Optional "expected envelope" band (e.g. the plausible speed range for
    // the truck's current haul-cycle phase). When bandMaxValue > bandMinValue
    // the band is painted as a pale arc segment behind the value arc so an
    // out-of-envelope reading is visible at a glance, not just via the alarm
    // colour.
    property real bandMinValue: 0
    property real bandMaxValue: 0

    readonly property real startAngle: 135 * Math.PI / 180
    readonly property real sweepAngle: 270 * Math.PI / 180
    readonly property real fraction: Math.max(0, Math.min(1, (value - minValue) / (maxValue - minValue)))
    readonly property color effectiveColor: alarm ? "#e5484d" : accentColor

    onValueChanged: canvas.requestPaint()
    onAlarmChanged: canvas.requestPaint()
    onBandMinValueChanged: canvas.requestPaint()
    onBandMaxValueChanged: canvas.requestPaint()

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

            if (root.bandMaxValue > root.bandMinValue) {
                const bandStartFraction = Math.max(0, Math.min(1, (root.bandMinValue - root.minValue) / (root.maxValue - root.minValue)));
                const bandEndFraction = Math.max(0, Math.min(1, (root.bandMaxValue - root.minValue) / (root.maxValue - root.minValue)));
                ctx.lineWidth = radius * 0.14;
                ctx.strokeStyle = "#3a4a3f";
                ctx.beginPath();
                ctx.arc(cx, cy, radius, root.startAngle + root.sweepAngle * bandStartFraction,
                        root.startAngle + root.sweepAngle * bandEndFraction, false);
                ctx.stroke();
            }

            ctx.lineWidth = radius * 0.14;
            ctx.strokeStyle = root.effectiveColor;
            ctx.beginPath();
            ctx.arc(cx, cy, radius, root.startAngle, root.startAngle + root.sweepAngle * root.fraction, false);
            ctx.stroke();

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
            color: root.alarm ? "#e5484d" : "#f2f4f8"
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
