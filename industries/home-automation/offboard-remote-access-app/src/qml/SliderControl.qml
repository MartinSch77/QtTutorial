// SPDX-License-Identifier: MIT
import QtQuick

Item {
    id: root

    property real from: 0
    property real to: 100
    property real value: 0
    property color accentColor: "#39c0ff"
    property bool enabled: true

    signal moved(real value)

    implicitHeight: 20

    function valueFromX(x) {
        const ratio = Math.max(0, Math.min(1, x / root.width));
        return root.from + ratio * (root.to - root.from);
    }

    Rectangle {
        id: track
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: 6
        radius: 3
        color: "#2a3140"
    }

    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        width: track.width * ((root.value - root.from) / Math.max(1, (root.to - root.from)))
        height: 6
        radius: 3
        color: root.enabled ? root.accentColor : "#4a5568"
    }

    Rectangle {
        id: handle
        width: 22
        height: 22
        radius: 11
        color: "white"
        border.color: root.enabled ? root.accentColor : "#4a5568"
        border.width: 2
        anchors.verticalCenter: parent.verticalCenter
        x: track.width * ((root.value - root.from) / Math.max(1, (root.to - root.from))) - width / 2
    }

    MouseArea {
        anchors.fill: parent
        enabled: root.enabled
        onPressed: root.value = root.valueFromX(mouseX);
        onPositionChanged: if (pressed) root.value = root.valueFromX(mouseX);
        onReleased: root.moved(root.value)
    }
}
