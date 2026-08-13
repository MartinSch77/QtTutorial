// SPDX-License-Identifier: MIT
import QtQuick

// A vector turn-signal arrow, hand-painted on a Canvas as a chevron stack (the same
// visual language as a real cluster's turn-signal telltale) instead of a plain glyph
// character. direction: -1 = left-pointing, 1 = right-pointing.
Item {
    id: root

    property bool active: false
    property int direction: 1 // -1 left, 1 right
    property color onColor: "#3ddc6f"

    implicitWidth: 40
    implicitHeight: 28

    opacity: active ? 1.0 : 0.15
    Behavior on opacity { NumberAnimation { duration: 100 } }

    Canvas {
        id: canvas
        anchors.fill: parent

        Connections {
            target: root
            function onActiveChanged() { canvas.requestPaint(); }
        }

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            ctx.fillStyle = root.onColor;

            const chevronWidth = width / 3.2;
            const chevronHeight = height * 0.7;
            const gap = width * 0.22;
            const cy = height / 2;

            for (let i = 0; i < 3; ++i) {
                const cx = root.direction > 0
                    ? i * gap
                    : width - i * gap;
                const sign = root.direction;
                ctx.beginPath();
                ctx.moveTo(cx - sign * chevronWidth / 2, cy - chevronHeight / 2);
                ctx.lineTo(cx + sign * chevronWidth / 2, cy);
                ctx.lineTo(cx - sign * chevronWidth / 2, cy + chevronHeight / 2);
                ctx.lineTo(cx - sign * chevronWidth / 2 + sign * chevronWidth * 0.35, cy);
                ctx.closePath();
                ctx.fill();
            }
        }
    }
}
