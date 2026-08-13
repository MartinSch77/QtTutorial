// SPDX-License-Identifier: MIT
import QtQuick

// LCARS-styled station switcher: a vertical sidebar of pill buttons on wide
// (desktop-ish) layouts, the same buttons laid out as a horizontal bottom
// tab strip on narrow (phone-ish) layouts. Driven purely by the "horizontal"
// property so Main.qml can reflow it at different widths.
Rectangle {
    id: root
    property bool horizontal: false
    property int currentIndex: 0
    property color accentColor: "#ff9c00"
    readonly property var stationNames: [qsTr("Viewscreen"), qsTr("Helm"), qsTr("Engineering"), qsTr("Tactical")]

    color: "#15111a"

    Grid {
        anchors.fill: parent
        anchors.margins: 10
        columns: root.horizontal ? root.stationNames.length : 1
        spacing: 8
        flow: root.horizontal ? Grid.LeftToRight : Grid.TopToBottom

        Repeater {
            model: root.stationNames
            LcarsButton {
                width: root.horizontal ? (root.width - 10 * (root.stationNames.length + 1)) / root.stationNames.length
                                       : root.width - 20
                height: root.horizontal ? root.height - 20 : 46
                label: modelData
                selected: index === root.currentIndex
                activeColor: root.accentColor
                onClicked: root.currentIndex = index
            }
        }
    }
}
