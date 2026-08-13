// SPDX-License-Identifier: MIT
import QtQuick

// A padlock glyph: the shackle sits closed over the body when locked and
// swings open to one side when unlocked, drawn as plain geometric paths on a
// Canvas - no icon font or external asset. See onboard-wall-panel's
// LockIcon.qml for the sibling copy.
Item {
    id: root

    property bool locked: true
    property color lockedColor: "#3ddc6f"
    property color unlockedColor: "#c0392b"

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
            const color = root.locked ? root.lockedColor : root.unlockedColor;

            ctx.strokeStyle = color;
            ctx.fillStyle = color;
            ctx.lineWidth = 2.2;
            ctx.lineCap = "round";

            const shackleR = w * 0.24;
            const shackleCx = root.locked ? w * 0.5 : w * 0.36;
            const shackleTop = h * 0.28;
            ctx.beginPath();
            ctx.arc(shackleCx, shackleTop, shackleR, Math.PI, 0, false);
            ctx.stroke();
            ctx.beginPath();
            ctx.moveTo(shackleCx - shackleR, shackleTop);
            ctx.lineTo(shackleCx - shackleR, shackleTop + h * 0.14);
            ctx.stroke();
            ctx.beginPath();
            ctx.moveTo(shackleCx + shackleR, shackleTop);
            ctx.lineTo(shackleCx + shackleR, shackleTop + h * 0.14);
            ctx.stroke();

            const bodyY = h * 0.46;
            const bodyH = h * 0.42;
            ctx.beginPath();
            ctx.roundedRect(w * 0.22, bodyY, w * 0.56, bodyH, 4, 4);
            ctx.fill();

            ctx.fillStyle = "#0d1117";
            ctx.beginPath();
            ctx.arc(w * 0.5, bodyY + bodyH * 0.4, w * 0.06, 0, Math.PI * 2);
            ctx.fill();
            ctx.fillRect(w * 0.47, bodyY + bodyH * 0.4, w * 0.06, bodyH * 0.35);
        }
    }

    Component.onCompleted: canvas.requestPaint()
    onLockedChanged: canvas.requestPaint()
}
