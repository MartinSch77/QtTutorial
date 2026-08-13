// SPDX-License-Identifier: MIT
import QtQuick

// A door-in-a-frame glyph: the door swings open away from the frame while
// `triggered` is true (open/motion detected) and sits flush shut otherwise -
// plain geometric paths drawn on a Canvas, no icon font or external asset.
Item {
    id: root

    property bool triggered: false
    property color idleColor: "#3ddc6f"
    property color triggeredColor: "#d4a12b"

    implicitWidth: 24
    implicitHeight: 24

    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const w = width;
            const h = height;
            const color = root.triggered ? root.triggeredColor : root.idleColor;

            ctx.strokeStyle = color;
            ctx.lineWidth = 2;
            ctx.lineJoin = "round";

            // Frame.
            ctx.strokeRect(w * 0.14, h * 0.1, w * 0.72, h * 0.82);

            // Door: hinged at the left edge of the frame, swung open by an
            // angle proportional to whether it is triggered.
            const hingeX = w * 0.18;
            const hingeY = h * 0.92;
            const doorLen = h * 0.78;
            const angle = root.triggered ? -Math.PI * 0.32 : -Math.PI * 0.5;
            const doorX = hingeX + Math.cos(angle) * doorLen;
            const doorY = hingeY + Math.sin(angle) * doorLen;

            ctx.beginPath();
            ctx.moveTo(hingeX, hingeY);
            ctx.lineTo(doorX, doorY);
            ctx.stroke();

            // Doorknob.
            ctx.fillStyle = color;
            ctx.beginPath();
            ctx.arc(hingeX + (doorX - hingeX) * 0.85, hingeY + (doorY - hingeY) * 0.85, w * 0.035, 0, Math.PI * 2);
            ctx.fill();
        }
    }

    Component.onCompleted: canvas.requestPaint()
    onTriggeredChanged: canvas.requestPaint()
}
