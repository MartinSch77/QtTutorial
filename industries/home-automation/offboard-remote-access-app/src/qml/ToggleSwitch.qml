// SPDX-License-Identifier: MIT
import QtQuick

Rectangle {
    id: root

    property bool checked: false
    property color onColor: "#3ddc6f"

    signal toggled(bool checked)

    width: 52
    height: 28
    radius: height / 2
    color: checked ? onColor : "#2a3140"
    border.color: checked ? onColor : "#4a5568"
    border.width: 1

    Behavior on color { ColorAnimation { duration: 150 } }

    Rectangle {
        id: knob
        width: parent.height - 6
        height: width
        radius: width / 2
        anchors.verticalCenter: parent.verticalCenter
        x: root.checked ? root.width - width - 3 : 3
        color: "white"

        Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.OutQuad } }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.checked = !root.checked;
            root.toggled(root.checked);
        }
    }
}
