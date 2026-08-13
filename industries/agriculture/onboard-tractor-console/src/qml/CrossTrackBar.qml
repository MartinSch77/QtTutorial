// SPDX-License-Identifier: MIT
import QtQuick

// GPS-guided steering-assist cross-track error indicator: a horizontal scale
// centred on the planted row line with a moving marker showing the current
// lateral deviation. Hand-painted on a Canvas so no chart dependency is needed.
Item {
    id: root

    property real value: 0
    property real maxValue: 15
    implicitHeight: 90

    onValueChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const midY = height * 0.45;
            const trackLeft = 12;
            const trackRight = width - 12;
            const trackWidth = trackRight - trackLeft;

            ctx.strokeStyle = "#2a2f3a";
            ctx.lineWidth = 6;
            ctx.beginPath();
            ctx.moveTo(trackLeft, midY);
            ctx.lineTo(trackRight, midY);
            ctx.stroke();

            ctx.strokeStyle = "#3ddc6f";
            ctx.lineWidth = 2;
            ctx.beginPath();
            ctx.moveTo(trackLeft + trackWidth / 2, midY - 14);
            ctx.lineTo(trackLeft + trackWidth / 2, midY + 14);
            ctx.stroke();

            const fraction = Math.max(-1, Math.min(1, root.value / root.maxValue));
            const markerX = trackLeft + trackWidth / 2 + fraction * (trackWidth / 2);

            ctx.fillStyle = Math.abs(root.value) > root.maxValue * 0.6 ? "#e5484d" : "#39c0ff";
            ctx.beginPath();
            ctx.moveTo(markerX, midY - 18);
            ctx.lineTo(markerX - 8, midY - 32);
            ctx.lineTo(markerX + 8, midY - 32);
            ctx.closePath();
            ctx.fill();
        }
    }

    Text {
        anchors.top: canvas.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#9aa4b2"
        font.pixelSize: 13
        text: qsTr("cross-track: %1 cm").arg(root.value.toFixed(1))
    }
}
