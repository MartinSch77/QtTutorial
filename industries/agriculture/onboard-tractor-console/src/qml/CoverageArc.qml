// SPDX-License-Identifier: MIT
import QtQuick

// Field-coverage indicator: a full-circle arc that fills as the current pass
// progresses towards 100%, with the current pass number shown at centre.
Item {
    id: root

    property real percent: 0
    property int passNumber: 1

    readonly property real startAngle: -90 * Math.PI / 180
    readonly property real fraction: Math.max(0, Math.min(1, percent / 100))

    onPercentChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const cx = width / 2;
            const cy = height / 2;
            const radius = Math.min(width, height) / 2 - 8;

            ctx.lineWidth = radius * 0.16;
            ctx.strokeStyle = "#2a2f3a";
            ctx.beginPath();
            ctx.arc(cx, cy, radius, 0, 2 * Math.PI, false);
            ctx.stroke();

            ctx.strokeStyle = "#e5b93d";
            ctx.beginPath();
            ctx.arc(cx, cy, radius, root.startAngle, root.startAngle + 2 * Math.PI * root.fraction, false);
            ctx.stroke();
        }
    }

    Column {
        anchors.centerIn: parent
        spacing: 2

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: Math.round(root.percent) + "%"
            color: "#f2f4f8"
            font.pixelSize: root.height * 0.16
            font.bold: true
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("pass %1").arg(root.passNumber)
            color: "#9aa4b2"
            font.pixelSize: root.height * 0.08
        }
    }
}
