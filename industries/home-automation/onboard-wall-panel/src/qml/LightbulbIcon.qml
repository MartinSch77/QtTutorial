// SPDX-License-Identifier: MIT
import QtQuick

// A lightbulb glyph whose fill/glow reflects on/off/dimmed state, drawn as
// plain geometric paths on a Canvas - no icon font or external asset.
Item {
    id: root

    property bool on: false
    // 0-100; only meaningful while `on` is true. Drives both the glow
    // radius and the fill opacity so a dim light visibly looks dimmer.
    property int brightness: 100
    property color onColor: "#f4c860"
    property color offColor: "#4a5568"

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
            const cx = w / 2;
            const cy = h * 0.42;
            const r = w * 0.32;
            const fillOpacity = root.on ? Math.max(0.25, root.brightness / 100) : 0;

            if (root.on) {
                const glow = ctx.createRadialGradient(cx, cy, r * 0.4, cx, cy, r * 1.9);
                glow.addColorStop(0, Qt.rgba(root.onColor.r, root.onColor.g, root.onColor.b, 0.55 * fillOpacity));
                glow.addColorStop(1, Qt.rgba(root.onColor.r, root.onColor.g, root.onColor.b, 0));
                ctx.fillStyle = glow;
                ctx.beginPath();
                ctx.arc(cx, cy, r * 1.9, 0, Math.PI * 2);
                ctx.fill();
            }

            const glass = root.on ? root.onColor : root.offColor;
            ctx.strokeStyle = glass;
            ctx.lineWidth = 2;
            ctx.fillStyle = Qt.rgba(glass.r, glass.g, glass.b, root.on ? fillOpacity : 0);

            // Bulb globe.
            ctx.beginPath();
            ctx.arc(cx, cy, r, 0, Math.PI * 2);
            ctx.fill();
            ctx.stroke();

            // Filament base.
            ctx.beginPath();
            ctx.moveTo(cx - r * 0.4, cy + r * 0.85);
            ctx.lineTo(cx + r * 0.4, cy + r * 0.85);
            ctx.lineTo(cx + r * 0.3, cy + r * 1.5);
            ctx.lineTo(cx - r * 0.3, cy + r * 1.5);
            ctx.closePath();
            ctx.stroke();
        }
    }

    Component.onCompleted: canvas.requestPaint()
    onOnChanged: canvas.requestPaint()
    onBrightnessChanged: canvas.requestPaint()
}
