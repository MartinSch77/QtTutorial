// SPDX-License-Identifier: MIT
import QtQuick

// A tank/silo glyph: a rectangular body with a tapered hopper bottom, drawn
// as a Canvas path (not a plain rectangle), plus a colour-coded fill-level
// indicator clipped to that same bounding box.
Item {
    id: root
    property real levelPercent: 0.0 // 0..100
    property string label: ""
    implicitWidth: 120
    implicitHeight: 200

    readonly property real hopperHeight: root.height * 0.16
    readonly property color fillColor: levelPercent > 90 ? "#e5484d" : (levelPercent < 10 ? "#e5b93d" : "#2f81f7")

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        text: root.label
        color: "#c8d0da"
        font.pixelSize: 11
    }

    Item {
        id: body
        anchors.horizontalCenter: parent.horizontalCenter
        y: 16
        width: parent.width
        height: parent.height - 16

        Canvas {
            id: silhouette
            anchors.fill: parent

            onWidthChanged: requestPaint()
            onHeightChanged: requestPaint()

            onPaint: {
                const ctx = getContext("2d");
                ctx.reset();
                const w = width;
                const h = height;
                const hopper = root.hopperHeight;

                ctx.fillStyle = "#0d1116";
                ctx.strokeStyle = "#5a6472";
                ctx.lineWidth = 2;
                ctx.lineJoin = "round";
                ctx.beginPath();
                ctx.moveTo(0, 0);
                ctx.lineTo(w, 0);
                ctx.lineTo(w, h - hopper);
                ctx.lineTo(w / 2, h);
                ctx.lineTo(0, h - hopper);
                ctx.closePath();
                ctx.fill();
                ctx.stroke();
            }
        }

        Item {
            id: fillClip
            anchors.fill: parent
            clip: true

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: parent.height * Math.min(Math.max(root.levelPercent, 0), 100) / 100
                color: root.fillColor

                Behavior on height { NumberAnimation { duration: 200 } }
            }
        }

        Text {
            anchors.centerIn: parent
            text: root.levelPercent.toFixed(1) + " %"
            color: "#f2f4f7"
            font.pixelSize: 15
            font.bold: true
        }
    }
}
