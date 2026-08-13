// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtGraphs
import QtTutorial.Factory.OffboardDigitalTwinControlCenter

// Real Qt Graphs charts for step 4 ("analyze the problem"): a live
// vibration waveform and historical temperature line (Qt Graphs 2D
// `GraphsView`/`LineSeries`, verified against
// framework-tour/09-latest-qt-release-features/qml/Main.qml, which already
// uses this exact `import QtGraphs` / `GraphsView` / `LineSeries` pattern
// against Qt 6.11.1 in CI) plus a `Surface3D` surface graph over the
// synthetic temperature grid (Surface3D/Surface3DSeries confirmed real via
// qt_documentation_read on qml-qtgraphs-surface3d.html and
// qml-qtgraphs-surface3dseries.html).
//
// This file is only ever added to the app's QML_SOURCES when Qt6::Graphs is
// actually found (see CMakeLists.txt, mirroring
// showcases/stock-tracker/qml/PriceChartGraphs.qml's guarded-inclusion
// pattern) - this repository's local Qt 6.4.2 baseline does not have Qt
// Graphs (needs Qt >= 6.9), so this file could not itself be built or run in
// this sandbox; ChartsFallback.qml is what actually renders here and on any
// other pre-6.9 Qt install. See README.md.
Item {
    id: root

    property var vibrationHistory: []
    property var temperatureHistory: []
    property var surfaceGrid: []

    Row {
        anchors.fill: parent
        spacing: 12

        GraphsView {
            id: waveformView
            width: (parent.width - 24) / 3
            height: parent.height
            theme: GraphsTheme { colorScheme: GraphsTheme.ColorScheme.Dark; seriesColors: [Theme.accentNormal] }
            axisX: ValueAxis { min: 0; max: Math.max(1, root.vibrationHistory.length) }
            axisY: ValueAxis { min: 0; max: 3 }
            LineSeries {
                id: vibrationSeries
                name: qsTr("Vibration")
            }
            Connections {
                target: root
                function onVibrationHistoryChanged() {
                    vibrationSeries.clear();
                    for (let i = 0; i < root.vibrationHistory.length; ++i)
                        vibrationSeries.append(i, root.vibrationHistory[i]);
                }
            }
        }

        GraphsView {
            id: historyView
            width: (parent.width - 24) / 3
            height: parent.height
            theme: GraphsTheme { colorScheme: GraphsTheme.ColorScheme.Dark; seriesColors: [Theme.accentWarning] }
            axisX: ValueAxis { min: 0; max: Math.max(1, root.temperatureHistory.length) }
            axisY: ValueAxis { min: 0; max: 100 }
            LineSeries {
                id: temperatureSeries
                name: qsTr("Temperature")
            }
            Connections {
                target: root
                function onTemperatureHistoryChanged() {
                    temperatureSeries.clear();
                    for (let i = 0; i < root.temperatureHistory.length; ++i)
                        temperatureSeries.append(i, root.temperatureHistory[i]);
                }
            }
        }

        Surface3D {
            id: surface
            width: (parent.width - 24) / 3
            height: parent.height
            theme: GraphsTheme { colorScheme: GraphsTheme.ColorScheme.Dark }
            axisX: Value3DAxis { title: qsTr("X") }
            axisY: Value3DAxis { title: qsTr("Temperature") }
            axisZ: Value3DAxis { title: qsTr("Z") }

            Surface3DSeries {
                id: surfaceSeries
                shading: Surface3DSeries.Shading.Smooth
                drawMode: Surface3DSeries.DrawFlag.DrawSurface
            }

            Connections {
                target: root
                function onSurfaceGridChanged() {
                    const rows = [];
                    for (let r = 0; r < root.surfaceGrid.length; ++r) {
                        const row = [];
                        for (let c = 0; c < root.surfaceGrid[r].length; ++c)
                            row.push(Qt.vector3d(c, root.surfaceGrid[r][c], r));
                        rows.push(row);
                    }
                    surfaceSeries.dataArray = rows;
                }
            }
        }
    }
}
