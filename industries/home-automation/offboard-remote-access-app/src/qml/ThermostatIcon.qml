// SPDX-License-Identifier: MIT
import QtQuick

// A small thermostat-dial glyph, drawn as plain geometric paths on a
// Canvas. See onboard-wall-panel's ThermostatIcon.qml for the sibling copy.
Item {
    id: root

    property int mode: 0 // 0 = Off, 1 = Heat, 2 = Cool
    property color offColor: "#4a5568"
    property color heatColor: "#ff9f43"
    property color coolColor: "#39c0ff"

    implicitWidth: 24
    implicitHeight: 24

    readonly property color activeColor: root.mode === 1 ? root.heatColor : (root.mode === 2 ? root.coolColor : root.offColor)

    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const w = width;
            const h = height;
            const cx = w / 2;
            const cy = h / 2;
            const r = Math.min(w, h) * 0.42;

            ctx.strokeStyle = "#2a3140";
            ctx.lineWidth = 3;
            ctx.beginPath();
            ctx.arc(cx, cy, r, Math.PI * 0.75, Math.PI * 2.25);
            ctx.stroke();

            ctx.strokeStyle = root.activeColor;
            const sweep = root.mode === 0 ? 0.0 : 0.65;
            ctx.beginPath();
            ctx.arc(cx, cy, r, Math.PI * 0.75, Math.PI * 0.75 + sweep * (Math.PI * 1.5));
            ctx.stroke();

            ctx.fillStyle = root.activeColor;
            ctx.beginPath();
            ctx.arc(cx, cy, r * 0.28, 0, Math.PI * 2);
            ctx.fill();
        }
    }

    Component.onCompleted: canvas.requestPaint()
    onModeChanged: canvas.requestPaint()
}
