// SPDX-License-Identifier: MIT
import QtQuick

// A top-down field-coverage map, in the spirit of a Gen4-style in-cab guidance
// display: the field is laid out as fieldRowCount horizontal strips (rows),
// covered boustrophedon-style (the tractor drives forward along a row, turns,
// then drives back along the next). Rows already fully driven are painted as
// a solid "covered" swath; the current row is filled up to the tractor's
// live progress within it, and empty rows remain unplanted soil colour.
// Hand-painted on a Canvas: no chart/image assets, fully resolution
// independent.
Item {
    id: root

    property real coveragePercent: 0     // progress within the current row/pass, 0..100
    property int rowIndex: 0             // 0-based current row
    property int fieldRowCount: 10
    property bool movingForward: true

    onCoveragePercentChanged: canvas.requestPaint()
    onRowIndexChanged: canvas.requestPaint()
    onMovingForwardChanged: canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();

            const marginX = width * 0.06;
            const marginY = height * 0.08;
            const fieldLeft = marginX;
            const fieldTop = marginY;
            const fieldWidth = width - 2 * marginX;
            const fieldHeight = height - 2 * marginY;
            const rowCount = Math.max(1, root.fieldRowCount);
            const rowHeight = fieldHeight / rowCount;

            // Unplanted soil background for the whole field boundary.
            ctx.fillStyle = "#2a2117";
            ctx.fillRect(fieldLeft, fieldTop, fieldWidth, fieldHeight);

            const fraction = Math.max(0, Math.min(1, root.coveragePercent / 100));

            for (let row = 0; row < rowCount; ++row) {
                const y = fieldTop + row * rowHeight;
                const forward = (row % 2 === 0);

                if (row < root.rowIndex) {
                    // Fully covered row.
                    ctx.fillStyle = "#2f5c2a";
                    ctx.fillRect(fieldLeft, y, fieldWidth, rowHeight);
                } else if (row === root.rowIndex) {
                    // Current row, partially covered from the entry side.
                    const coveredWidth = fieldWidth * fraction;
                    ctx.fillStyle = "#3ddc6f";
                    if (forward) {
                        ctx.fillRect(fieldLeft, y, coveredWidth, rowHeight);
                    } else {
                        ctx.fillRect(fieldLeft + fieldWidth - coveredWidth, y, coveredWidth, rowHeight);
                    }
                }

                // Row divider.
                ctx.strokeStyle = "#1c150f";
                ctx.lineWidth = 1;
                ctx.beginPath();
                ctx.moveTo(fieldLeft, y);
                ctx.lineTo(fieldLeft + fieldWidth, y);
                ctx.stroke();
            }

            // Field boundary.
            ctx.strokeStyle = "#5a7a4a";
            ctx.lineWidth = 2;
            ctx.strokeRect(fieldLeft, fieldTop, fieldWidth, fieldHeight);

            // Tractor marker on the current row.
            const markerY = fieldTop + (root.rowIndex + 0.5) * rowHeight;
            const markerX = root.movingForward
                ? fieldLeft + fieldWidth * fraction
                : fieldLeft + fieldWidth * (1 - fraction);
            ctx.fillStyle = "#f2f4f8";
            ctx.beginPath();
            ctx.arc(markerX, markerY, Math.max(3, rowHeight * 0.28), 0, 2 * Math.PI);
            ctx.fill();
        }
    }

    Text {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 4
        text: qsTr("row %1 / %2").arg(root.rowIndex + 1).arg(root.fieldRowCount)
        color: "#9aa4b2"
        font.pixelSize: Math.max(10, root.height * 0.05)
    }
}
