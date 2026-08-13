// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls.Basic
import QtGraphs

// Only ever loaded when Qt Graphs is available (StockTrackerBackend.graphsAvailable
// is a compile-time capability flag; qml/Main.qml Loaders this file only when it's
// true, and it is entirely excluded from the build's QML_SOURCES otherwise - see
// showcases/stock-tracker/CMakeLists.txt). Renders the selected symbol's simulated
// price history as an interactive Qt Graphs LineSeries, zoomable via the slider and
// pannable by dragging across the chart.
Item {
    id: root
    property var priceHistory: []
    property color lineColor: "#4ac1e0"
    property int windowSize: 40
    property int panOffset: 0

    function rebuild() {
        series.clear();
        const history = root.priceHistory;
        const total = history.length;
        if (total < 2) {
            return;
        }
        const clampedWindow = Math.min(root.windowSize, total);
        const maxOffset = Math.max(0, total - clampedWindow);
        root.panOffset = Math.max(0, Math.min(root.panOffset, maxOffset));

        const start = Math.max(0, total - clampedWindow - root.panOffset);
        const end = Math.min(total, start + clampedWindow);
        for (let i = start; i < end; ++i) {
            series.append(i, history[i]);
        }
        axisX.min = start;
        axisX.max = Math.max(start + 1, end - 1);

        let min = Math.min.apply(null, history.slice(start, end));
        let max = Math.max.apply(null, history.slice(start, end));
        if (max === min) {
            max = min + 1;
        }
        const padding = (max - min) * 0.1;
        axisY.min = min - padding;
        axisY.max = max + padding;
    }

    onPriceHistoryChanged: rebuild()
    onWindowSizeChanged: rebuild()
    onPanOffsetChanged: rebuild()

    Column {
        anchors.fill: parent
        spacing: 4

        GraphsView {
            id: chart
            width: parent.width
            height: parent.height - controlsRow.height - 4

            theme: GraphsTheme {
                colorScheme: GraphsTheme.ColorScheme.Dark
                seriesColors: [root.lineColor]
            }

            axisX: ValueAxis { id: axisX; titleText: qsTr("Tick") }
            axisY: ValueAxis { id: axisY; titleText: qsTr("Price") }

            LineSeries {
                id: series
            }

            MouseArea {
                anchors.fill: parent
                property real lastX: 0
                onPressed: mouse => lastX = mouse.x
                onPositionChanged: mouse => {
                    const dx = mouse.x - lastX;
                    lastX = mouse.x;
                    // Dragging right pans back into history (increases panOffset);
                    // dragging left pans forward towards the live edge.
                    root.panOffset = Math.max(0, root.panOffset + dx / 4);
                }
            }
        }

        Row {
            id: controlsRow
            width: parent.width
            spacing: 8

            Label {
                text: qsTr("Zoom")
                color: "#8fa0b3"
                anchors.verticalCenter: parent.verticalCenter
            }
            Slider {
                width: 160
                from: 10
                to: 90
                value: root.windowSize
                onMoved: root.windowSize = Math.round(value)
            }
            Label {
                text: qsTr("Drag chart to pan")
                color: "#5a6b80"
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
