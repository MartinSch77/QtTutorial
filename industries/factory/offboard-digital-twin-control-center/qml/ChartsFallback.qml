// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtTutorial.Factory.OffboardDigitalTwinControlCenter

// Fallback for step 4 ("analyze the problem") when Qt6::Graphs is not
// available (this repository's local Qt 6.4.2 baseline does not have it -
// Qt Graphs needs Qt >= 6.9, see docs/qa/licensing.md). Draws the same three
// visualizations - live vibration waveform, historical temperature area, and
// a temperature-over-position heatmap standing in for the Surface3D graph -
// using plain QtQuick Canvas (backed by QPainter), so there's always
// *something* real on screen rather than a blank panel. Real Qt Graphs
// (LineSeries + Surface3D) is used instead via GraphsCharts.qml whenever
// Qt6::Graphs is present - see ChartsPanel.qml's Loader.
Item {
    id: root

    property var vibrationHistory: []
    property var temperatureHistory: []
    property var surfaceGrid: [] // 2D array [row][col] of temperature samples

    Row {
        anchors.fill: parent
        spacing: 12

        Canvas {
            id: waveformCanvas
            width: (parent.width - 24) / 3
            height: parent.height
            onPaint: {
                const ctx = getContext("2d");
                ctx.reset();
                ctx.strokeStyle = Theme.accentNormal;
                ctx.lineWidth = 2;
                ctx.beginPath();
                const data = root.vibrationHistory;
                if (data.length > 1) {
                    for (let i = 0; i < data.length; ++i) {
                        const x = (i / (data.length - 1)) * width;
                        const y = height - Math.min(1, data[i] / 3.0) * height;
                        if (i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
                    }
                }
                ctx.stroke();
            }
            Connections { target: root; function onVibrationHistoryChanged() { waveformCanvas.requestPaint(); } }
            Text { text: qsTr("Vibration waveform"); color: Theme.textSecondary; font.pixelSize: 11; anchors.top: parent.top }
        }

        Canvas {
            id: historyCanvas
            width: (parent.width - 24) / 3
            height: parent.height
            onPaint: {
                const ctx = getContext("2d");
                ctx.reset();
                const data = root.temperatureHistory;
                ctx.fillStyle = Qt.rgba(Theme.accentWarning.r, Theme.accentWarning.g, Theme.accentWarning.b, 0.35);
                ctx.strokeStyle = Theme.accentWarning;
                ctx.lineWidth = 2;
                ctx.beginPath();
                ctx.moveTo(0, height);
                if (data.length > 1) {
                    for (let i = 0; i < data.length; ++i) {
                        const x = (i / (data.length - 1)) * width;
                        const y = height - Math.min(1, data[i] / 100.0) * height;
                        ctx.lineTo(x, y);
                    }
                }
                ctx.lineTo(width, height);
                ctx.closePath();
                ctx.fill();
                ctx.stroke();
            }
            Connections { target: root; function onTemperatureHistoryChanged() { historyCanvas.requestPaint(); } }
            Text { text: qsTr("Temperature history"); color: Theme.textSecondary; font.pixelSize: 11; anchors.top: parent.top }
        }

        Canvas {
            id: surfaceCanvas
            width: (parent.width - 24) / 3
            height: parent.height
            onPaint: {
                const ctx = getContext("2d");
                ctx.reset();
                const grid = root.surfaceGrid;
                const rows = grid.length;
                if (rows === 0) return;
                const cols = grid[0].length;
                const cw = width / cols;
                const ch = height / rows;
                for (let r = 0; r < rows; ++r) {
                    for (let c = 0; c < cols; ++c) {
                        const t = Math.max(0, Math.min(1, (grid[r][c] - 20) / 80));
                        ctx.fillStyle = Qt.rgba(t, 1 - t, 0.3, 1);
                        ctx.fillRect(c * cw, r * ch, cw - 1, ch - 1);
                    }
                }
            }
            Connections { target: root; function onSurfaceGridChanged() { surfaceCanvas.requestPaint(); } }
            Text { text: qsTr("Temperature surface (2D stand-in)"); color: Theme.textSecondary; font.pixelSize: 11; anchors.top: parent.top }
        }
    }
}
