// SPDX-License-Identifier: MIT
import QtQuick

// Hand-painted (Canvas), not a bundled image: recreates the *shape language*
// of the iconic sweeping "elbow" bracket used throughout LCARS-style
// interfaces - a thick bar rounding through a single big quarter-circle
// sweep into a perpendicular bar - without reproducing any specific
// franchise's exact artwork, colors-as-branding, or text.
Item {
    id: root
    property color barColor: "#ff9c00"
    property real barThickness: 26
    property real elbowSize: 110
    default property alias content: contentItem.children

    Canvas {
        id: canvas
        anchors.fill: parent

        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()
        Connections {
            target: root
            function onBarColorChanged() { canvas.requestPaint(); }
        }

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            ctx.fillStyle = root.barColor;

            const bt = root.barThickness;
            const es = root.elbowSize;
            const cx = es;
            const cy = es;
            const rightMargin = 12;
            const bottomMargin = 12;

            ctx.beginPath();
            ctx.moveTo(width - rightMargin, 0);
            ctx.lineTo(es, 0);
            ctx.arc(cx, cy, es, -Math.PI / 2, Math.PI, true);
            ctx.lineTo(0, height - bottomMargin);
            ctx.lineTo(bt, height - bottomMargin);
            ctx.lineTo(bt, es);
            ctx.arc(cx, cy, Math.max(es - bt, 1), Math.PI, -Math.PI / 2, false);
            ctx.lineTo(width - rightMargin, bt);
            ctx.closePath();
            ctx.fill();

            ctx.fillRect(width - rightMargin, 0, rightMargin, bt);
            ctx.fillRect(0, height - bottomMargin, bt, bottomMargin);
        }
    }

    Item {
        id: contentItem
        anchors.fill: parent
        anchors.topMargin: root.barThickness + 6
        anchors.leftMargin: root.barThickness + 6
        anchors.rightMargin: 12
        anchors.bottomMargin: 12
    }
}
