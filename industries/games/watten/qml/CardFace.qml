// SPDX-License-Identifier: MIT
import QtQuick

// A hand-painted German-suited playing card: no image assets, just a
// Rectangle plus a small Canvas glyph per suit (Eichel/Gras/Herz/Schellen),
// coloured distinctly so all 4 suits stay readable at table-tile size.
Rectangle {
    id: root

    property string suit: "Herz"
    property string rank: "S"
    property bool highlighted: false
    property bool faceDown: false

    width: 64
    height: 92
    radius: 8
    color: faceDown ? "#2b3542" : "#fbf6ea"
    border.color: highlighted ? "#f4c542" : "#2b2118"
    border.width: highlighted ? 3 : 1.5

    readonly property color suitColor: {
        switch (suit) {
        case "Herz": return "#b5342d";
        case "Schellen": return "#c79a2c";
        case "Gras": return "#3f7d3a";
        default: return "#6b4a2b"; // Eichel
        }
    }

    Text {
        visible: !root.faceDown
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 6
        text: root.rank
        font.pixelSize: 16
        font.bold: true
        color: root.suitColor
    }

    Canvas {
        id: glyph
        visible: !root.faceDown
        anchors.centerIn: parent
        width: 34
        height: 34
        property color paintColor: root.suitColor
        property string paintSuit: root.suit
        onPaintColorChanged: requestPaint()
        onPaintSuitChanged: requestPaint()
        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            ctx.fillStyle = paintColor;
            ctx.strokeStyle = paintColor;
            const w = width;
            const h = height;
            switch (paintSuit) {
            case "Herz":
                ctx.beginPath();
                ctx.arc(w * 0.32, h * 0.35, w * 0.22, 0, Math.PI * 2);
                ctx.arc(w * 0.68, h * 0.35, w * 0.22, 0, Math.PI * 2);
                ctx.moveTo(w * 0.12, h * 0.42);
                ctx.lineTo(w * 0.5, h * 0.92);
                ctx.lineTo(w * 0.88, h * 0.42);
                ctx.closePath();
                ctx.fill();
                break;
            case "Schellen":
                ctx.beginPath();
                ctx.arc(w * 0.5, h * 0.55, w * 0.32, 0, Math.PI * 2);
                ctx.fill();
                ctx.beginPath();
                ctx.arc(w * 0.5, h * 0.2, w * 0.1, 0, Math.PI * 2);
                ctx.fill();
                ctx.beginPath();
                ctx.arc(w * 0.5, h * 0.62, w * 0.08, 0, Math.PI * 2);
                ctx.fillStyle = "#fbf6ea";
                ctx.fill();
                break;
            case "Gras":
                ctx.beginPath();
                ctx.moveTo(w * 0.5, h * 0.05);
                ctx.quadraticCurveTo(w * 0.95, h * 0.35, w * 0.5, h * 0.95);
                ctx.quadraticCurveTo(w * 0.05, h * 0.35, w * 0.5, h * 0.05);
                ctx.closePath();
                ctx.fill();
                break;
            default: // Eichel
                ctx.beginPath();
                ctx.arc(w * 0.5, h * 0.6, w * 0.28, 0, Math.PI * 2);
                ctx.fill();
                ctx.fillRect(w * 0.4, h * 0.08, w * 0.2, h * 0.22);
                break;
            }
        }
    }

    Text {
        visible: !root.faceDown
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 6
        rotation: 180
        text: root.rank
        font.pixelSize: 16
        font.bold: true
        color: root.suitColor
    }
}
