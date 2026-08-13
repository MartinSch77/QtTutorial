// SPDX-License-Identifier: MIT
import QtQuick

// A generic pitched-roof house glyph, hand-painted on a Canvas (see
// ThermostatGauge.qml for the precedent) rather than fetched from an icon
// font or SVG asset - this is a plain geometric silhouette, not any
// particular vendor's mark.
Item {
    id: root

    property color strokeColor: "#e6edf3"
    property real strokeWidth: 2

    implicitWidth: 28
    implicitHeight: 28

    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            const w = width;
            const h = height;

            ctx.strokeStyle = root.strokeColor;
            ctx.lineWidth = root.strokeWidth;
            ctx.lineJoin = "round";
            ctx.lineCap = "round";

            // Roof.
            ctx.beginPath();
            ctx.moveTo(w * 0.5, h * 0.08);
            ctx.lineTo(w * 0.92, h * 0.46);
            ctx.lineTo(w * 0.78, h * 0.46);
            ctx.lineTo(w * 0.78, h * 0.34);
            ctx.lineTo(w * 0.5, h * 0.08);
            ctx.lineTo(w * 0.08, h * 0.46);
            ctx.stroke();

            // Body.
            ctx.strokeRect(w * 0.2, h * 0.46, w * 0.6, h * 0.46);

            // Door.
            ctx.strokeRect(w * 0.44, h * 0.66, w * 0.14, h * 0.26);
        }
    }

    Component.onCompleted: canvas.requestPaint()
    onStrokeColorChanged: canvas.requestPaint()
}
