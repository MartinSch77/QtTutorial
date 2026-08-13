// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Shapes

// Fallback chart used when Qt Graphs is not available on this Qt (< 6.9): a
// hand-rolled QML Shapes line chart over the exact same C++-computed
// `priceHistory` array that PriceChartGraphs.qml (Qt Graphs LineSeries)
// consumes when Graphs IS available. Mapping price values to screen-space
// coordinates is ordinary view-layer transform code (no price simulation
// happens here), so it stays consistent with "QML only displays what C++
// computes".
Item {
    id: root
    property var priceHistory: []
    property color lineColor: "#4ac1e0"

    property var points: {
        const history = root.priceHistory;
        if (!history || history.length < 2 || root.width <= 0 || root.height <= 0) {
            return [];
        }
        let min = Math.min.apply(null, history);
        let max = Math.max.apply(null, history);
        if (max === min) {
            max = min + 1;
        }
        const margin = 8;
        const usableHeight = root.height - margin * 2;
        const stepX = root.width / (history.length - 1);
        const result = [];
        for (let i = 0; i < history.length; ++i) {
            const normalized = (history[i] - min) / (max - min);
            const x = i * stepX;
            const y = margin + (1 - normalized) * usableHeight;
            result.push(Qt.point(x, y));
        }
        return result;
    }

    Shape {
        anchors.fill: parent
        ShapePath {
            strokeColor: root.lineColor
            strokeWidth: 2
            fillColor: "transparent"
            joinStyle: ShapePath.RoundJoin

            PathPolyline {
                path: root.points
            }
        }
    }

    Text {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 4
        text: qsTr("Fallback chart (QML Shapes) — Qt Graphs not available on this Qt")
        color: "#5a6b80"
        font.pixelSize: 10
        visible: root.width > 260
    }
}
