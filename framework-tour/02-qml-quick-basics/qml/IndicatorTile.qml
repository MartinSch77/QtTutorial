// SPDX-License-Identifier: MIT
import QtQuick

Item {
    id: root
    property string indicatorName: ""
    property real indicatorValue: 0
    property string indicatorStatus: "nominal"
    height: 64

    Rectangle {
        id: tile
        anchors.fill: parent
        radius: 8
        color: "#1c232c"
        border.width: 2
        border.color: "#1c232c"
        state: root.indicatorStatus

        Row {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 12

            Text {
                text: root.indicatorName
                color: "white"
                font.pixelSize: 15
                width: parent.width * 0.4
                elide: Text.ElideRight
            }

            Rectangle {
                width: parent.width * 0.35
                height: 8
                radius: 4
                color: "#0d1117"
                anchors.verticalCenter: parent.verticalCenter

                Rectangle {
                    id: bar
                    height: parent.height
                    radius: 4
                    width: parent.width * Math.min(root.indicatorValue / 100, 1.0)
                    color: "#3ddc84"

                    Behavior on width {
                        NumberAnimation { duration: 400; easing.type: Easing.OutCubic }
                    }
                }
            }

            Text {
                text: Math.round(root.indicatorValue) + "%"
                color: "#8fa0b3"
                font.pixelSize: 13
            }
        }

        states: [
            State {
                name: "nominal"
                PropertyChanges { target: tile; border.color: "#1c232c" }
                PropertyChanges { target: bar; color: "#3ddc84" }
            },
            State {
                name: "warning"
                PropertyChanges { target: tile; border.color: "#e0a94a" }
                PropertyChanges { target: bar; color: "#e0a94a" }
            },
            State {
                name: "critical"
                PropertyChanges { target: tile; border.color: "#e0524a" }
                PropertyChanges { target: bar; color: "#e0524a" }
            }
        ]

        transitions: [
            Transition {
                ColorAnimation { properties: "border.color,color"; duration: 300 }
            }
        ]

        SequentialAnimation on scale {
            running: root.indicatorStatus === "critical"
            loops: Animation.Infinite
            NumberAnimation { to: 1.03; duration: 400; easing.type: Easing.InOutQuad }
            NumberAnimation { to: 1.0; duration: 400; easing.type: Easing.InOutQuad }
        }
    }
}
