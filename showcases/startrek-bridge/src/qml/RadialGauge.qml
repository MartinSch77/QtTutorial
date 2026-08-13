// SPDX-License-Identifier: MIT
import QtQuick

// A hand-painted (Canvas) radial gauge - no Qt Charts/Graphs, both of which
// are GPL/commercial-only and unsuitable for this MIT-licensed tutorial.
Item {
    id: root
    property real value: 0
    property real minValue: 0
    property real maxValue: 100
    property string label: ""
    property string valueText: ""
    property color accentColor: "#ff9c00"

    Canvas {
        id: canvas
        anchors.fill: parent

        property real fraction: (root.value - root.minValue) / Math.max(root.maxValue - root.minValue, 0.0001)
        onFractionChanged: requestPaint()
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()
        Connections {
            target: root
            function onAccentColorChanged() { canvas.requestPaint(); }
        }

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const cx = width / 2;
            const cy = height / 2;
            const radius = Math.min(width, height) / 2 - 8;
            const startAngle = Math.PI * 0.75;
            const endAngle = Math.PI * 2.25;
            const clampedFraction = Math.max(0, Math.min(1, fraction));

            ctx.lineWidth = 10;
            ctx.strokeStyle = "#241c2e";
            ctx.beginPath();
            ctx.arc(cx, cy, radius, startAngle, endAngle);
            ctx.stroke();

            ctx.strokeStyle = root.accentColor;
            ctx.beginPath();
            ctx.arc(cx, cy, radius, startAngle, startAngle + (endAngle - startAngle) * clampedFraction);
            ctx.stroke();
        }
    }

    Column {
        anchors.centerIn: parent
        spacing: 2
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.valueText
            color: "#f2e9ff"
            font.pixelSize: 20
            font.bold: true
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.label
            color: "#9c8fae"
            font.pixelSize: 11
        }
    }
}
