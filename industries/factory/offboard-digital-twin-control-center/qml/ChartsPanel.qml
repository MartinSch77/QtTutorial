// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtTutorial.Factory.OffboardDigitalTwinControlCenter

// Bottom panel: owns the rolling telemetry history buffers and picks
// between the real Qt Graphs charts and the Canvas fallback, based on the
// `graphsAvailable` context property main.cpp sets from the
// OFFBOARD_DIGITAL_TWIN_HAVE_GRAPHS compile definition.
Rectangle {
    id: root
    color: Theme.glassSurface
    border.color: Theme.border
    border.width: 1
    radius: Theme.radius

    property var machine
    property int historyLength: 60

    property var vibrationHistory: []
    property var temperatureHistory: []
    property var surfaceGrid: []

    Timer {
        interval: 300
        running: true
        repeat: true
        onTriggered: {
            if (!root.machine)
                return;
            const v = root.vibrationHistory.slice();
            v.push(root.machine.vibration);
            if (v.length > root.historyLength) v.shift();
            root.vibrationHistory = v;

            const t = root.temperatureHistory.slice();
            t.push(root.machine.temperature);
            if (t.length > root.historyLength) t.shift();
            root.temperatureHistory = t;

            // Synthetic 8x8 temperature surface centered on the current
            // reading, for the Surface3D/2D-heatmap visualization.
            const grid = [];
            for (let r = 0; r < 8; ++r) {
                const row = [];
                for (let c = 0; c < 8; ++c) {
                    const dist = Math.hypot(r - 3.5, c - 3.5);
                    row.push(root.machine.temperature - dist * 4 + Math.sin(r + c + Date.now() / 500) * 2);
                }
                grid.push(row);
            }
            root.surfaceGrid = grid;
        }
    }

    // Deliberately a Loader with a string `source:` URL (resolved lazily at
    // runtime), not a `sourceComponent:` referencing the GraphsCharts type
    // directly - GraphsCharts.qml is only bundled into this app's QML module
    // when Qt6::Graphs was found at configure time (see CMakeLists.txt), so
    // eagerly referencing its type here would fail to resolve on this
    // repository's local Qt 6.4.2 baseline. Mirrors
    // showcases/stock-tracker/qml/DetailPanel.qml's identical
    // PriceChartGraphs.qml/PriceChartFallback.qml Loader.
    Loader {
        id: chartLoader
        anchors.fill: parent
        anchors.margins: Theme.spacingUnit
        source: (typeof graphsAvailable !== "undefined" && graphsAvailable)
            ? "GraphsCharts.qml" : "ChartsFallback.qml"
        onLoaded: {
            item.vibrationHistory = root.vibrationHistory;
            item.temperatureHistory = root.temperatureHistory;
            item.surfaceGrid = root.surfaceGrid;
        }
    }

    onVibrationHistoryChanged: if (chartLoader.item) chartLoader.item.vibrationHistory = vibrationHistory
    onTemperatureHistoryChanged: if (chartLoader.item) chartLoader.item.temperatureHistory = temperatureHistory
    onSurfaceGridChanged: if (chartLoader.item) chartLoader.item.surfaceGrid = surfaceGrid
}
