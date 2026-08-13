// SPDX-License-Identifier: MIT
import QtQuick

// A conveyor-belt glyph: a rounded belt outline with two end rollers, drawn
// with Canvas paths, plus a row of diagonal "product" dashes that scroll
// along the belt whenever `speed` is above zero — the same idea as the
// flow-pulse animation used on the pipes, applied to a moving belt instead
// of a moving fluid.
Item {
    id: root
    property real speed: 0.0
    implicitWidth: 120
    implicitHeight: 28

    readonly property color beltColor: speed > 0.01 ? "#3ddc84" : "#5a6472"

    onBeltColorChanged: canvas.requestPaint()
    onWidthChanged: canvas.requestPaint()
    onHeightChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const w = width;
            const h = height;
            const r = h / 2;

            ctx.strokeStyle = root.beltColor;
            ctx.lineWidth = 2;
            ctx.lineJoin = "round";
            ctx.beginPath();
            ctx.moveTo(r, 1);
            ctx.lineTo(w - r, 1);
            ctx.arc(w - r, r, r - 1, -Math.PI / 2, Math.PI / 2, false);
            ctx.lineTo(r, h - 1);
            ctx.arc(r, r, r - 1, Math.PI / 2, -Math.PI / 2, false);
            ctx.stroke();

            // End rollers.
            ctx.beginPath();
            ctx.ellipse(1, 1, h - 2, h - 2);
            ctx.ellipse(w - h + 1, 1, h - 2, h - 2);
            ctx.stroke();
        }
    }

    Item {
        id: beltMask
        anchors.fill: canvas
        clip: true

        Row {
            id: dashes
            spacing: 10
            y: canvas.height / 2 - 2
            x: -20

            Repeater {
                model: 12
                Rectangle {
                    width: 14
                    height: 4
                    radius: 2
                    rotation: -30
                    color: root.beltColor
                    opacity: 0.85
                }
            }

            NumberAnimation on x {
                running: root.speed > 0.01
                loops: Animation.Infinite
                from: -20
                to: -20 + 12 * (14 + 10)
                duration: Math.max(200, 1400 / Math.max(root.speed, 0.05))
            }
        }
    }
}
