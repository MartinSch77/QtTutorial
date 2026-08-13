// SPDX-License-Identifier: MIT
import QtQuick

// A pill-shaped LCARS-style selector button (uniform corner radius only -
// this tutorial targets Qt 6.4, and Rectangle's independent per-corner
// radius properties only exist from Qt 6.7).
Rectangle {
    id: root
    property string label: ""
    property bool selected: false
    property color activeColor: "#ff9c00"
    signal clicked()

    radius: height / 2
    color: selected ? activeColor : "#2a2430"
    border.width: selected ? 0 : 1
    border.color: activeColor

    Behavior on color { ColorAnimation { duration: 150 } }

    Text {
        anchors.centerIn: parent
        text: root.label
        color: root.selected ? "#0c0910" : activeColor
        font.pixelSize: 14
        font.bold: true
        font.letterSpacing: 1
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
