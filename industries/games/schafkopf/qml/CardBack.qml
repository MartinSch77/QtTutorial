// SPDX-License-Identifier: MIT
import QtQuick

Item {
    width: 76
    height: 108

    Rectangle {
        anchors.fill: parent
        radius: 8
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1c3d5a" }
            GradientStop { position: 1.0; color: "#0d1f2e" }
        }
        border.width: 2
        border.color: "#0a141d"

        Rectangle {
            anchors.centerIn: parent
            width: parent.width - 20
            height: parent.height - 20
            radius: 6
            color: "transparent"
            border.width: 2
            border.color: "#3a5d7a"
        }
    }
}
