// SPDX-License-Identifier: MIT
import QtQuick

// A simulated drone camera feed with object-detection bounding-box overlays.
// This is a passive visualization of what a detection pipeline's *output*
// looks like drawn over video - there is no real camera, no real ML model,
// and nothing here computes or displays a targeting/aim solution.
Rectangle {
    id: root
    property var sim
    color: "#0a0f14"
    clip: true

    Rectangle {
        id: sky
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1c3a52" }
            GradientStop { position: 1.0; color: "#2c4a34" }
        }

        property real driftPx: 0
        NumberAnimation on driftPx {
            from: 0; to: root.width + 60
            duration: 6000
            loops: Animation.Infinite
        }

        Repeater {
            model: 6
            Rectangle {
                width: 60
                height: 4
                radius: 2
                color: "#345a3f"
                x: (index * 180 + sky.driftPx * (1 + index * 0.1)) % (root.width + 60) - 60
                y: root.height * 0.55 + index * 12
            }
        }
    }

    Text {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 8
        text: qsTr("SIMULATED FEED - CAM 01")
        color: "#3ddc6f"
        font.pixelSize: 12
        font.bold: true
    }

    Repeater {
        model: root.sim ? root.sim.detections : []

        Item {
            x: modelData.x * root.width
            y: modelData.y * root.height
            width: modelData.w * root.width
            height: modelData.h * root.height

            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: "#ffcc00"
                border.width: 2
            }

            Rectangle {
                anchors.bottom: parent.top
                anchors.left: parent.left
                height: 18
                width: label.implicitWidth + 10
                color: "#ffcc00"

                Text {
                    id: label
                    anchors.centerIn: parent
                    text: modelData.label + " " + Math.round(modelData.confidence * 100) + "%"
                    color: "#1a1a1a"
                    font.pixelSize: 11
                    font.bold: true
                }
            }
        }
    }
}
