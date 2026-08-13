// SPDX-License-Identifier: MIT
import QtQuick

// Simulated first-person AR HUD overlay - stands in for AR glasses / a
// soldier heads-up display: a translucent compass strip, waypoint markers
// and teammate position indicators, evoking an augmented-reality overlay
// without any real AR hardware or tracking. Purely informational; nothing
// here is weapons- or targeting-related.
Rectangle {
    id: root
    property var sim
    color: "#05070a"

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#10151c" }
            GradientStop { position: 1.0; color: "#1a2230" }
        }
    }

    Rectangle {
        id: compassStrip
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 20
        height: 40
        color: "#2f3d4d"
        opacity: 0.55
        radius: 4

        Canvas {
            anchors.fill: parent
            onPaint: {
                const ctx = getContext("2d");
                ctx.reset();
                ctx.strokeStyle = "#9adfff";
                ctx.fillStyle = "#9adfff";
                ctx.font = "11px sans-serif";
                const heading = root.sim ? root.sim.hudHeadingDeg : 0;
                for (let d = -60; d <= 60; d += 15) {
                    const deg = ((heading + d) % 360 + 360) % 360;
                    const px = width / 2 + d * (width / 130);
                    ctx.beginPath();
                    ctx.moveTo(px, height * 0.6);
                    ctx.lineTo(px, height);
                    ctx.stroke();
                    ctx.fillText(Math.round(deg) + "°", px - 10, height * 0.45);
                }
                ctx.strokeStyle = "#ffcc00";
                ctx.beginPath();
                ctx.moveTo(width / 2, 0);
                ctx.lineTo(width / 2, height);
                ctx.stroke();
            }

            Timer {
                interval: 150
                running: true
                repeat: true
                onTriggered: parent.requestPaint()
            }
        }
    }

    Item {
        id: reticleField
        anchors.fill: parent
        anchors.topMargin: 80
        anchors.bottomMargin: 60

        Repeater {
            model: root.sim ? root.sim.waypoints : []
            Item {
                readonly property real angleRad: (modelData.bearingDeg - root.sim.hudHeadingDeg) * Math.PI / 180
                readonly property real screenX: reticleField.width / 2 + Math.sin(angleRad) * reticleField.width * 0.4
                visible: Math.abs(((modelData.bearingDeg - root.sim.hudHeadingDeg + 540) % 360) - 180) < 70
                x: screenX - 30
                y: reticleField.height * 0.35

                Column {
                    spacing: 2
                    Rectangle {
                        width: 16; height: 16; radius: 8
                        color: "transparent"
                        border.color: "#3ddc6f"
                        border.width: 2
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Text {
                        text: modelData.label + " " + modelData.distanceKm.toFixed(1) + "km"
                        color: "#3ddc6f"
                        font.pixelSize: 11
                    }
                }
            }
        }

        Repeater {
            model: root.sim ? root.sim.teammates : []
            Item {
                readonly property real angleRad: (modelData.bearingDeg - root.sim.hudHeadingDeg) * Math.PI / 180
                readonly property real screenX: reticleField.width / 2 + Math.sin(angleRad) * reticleField.width * 0.4
                visible: Math.abs(((modelData.bearingDeg - root.sim.hudHeadingDeg + 540) % 360) - 180) < 70
                x: screenX - 20
                y: reticleField.height * 0.55

                Column {
                    spacing: 2
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "▲"
                        color: "#39c0ff"
                        font.pixelSize: 16
                    }
                    Text {
                        text: "F-" + modelData.id + " " + modelData.distanceKm.toFixed(1) + "km"
                        color: "#39c0ff"
                        font.pixelSize: 11
                    }
                }
            }
        }
    }

    Text {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 12
        text: qsTr("SIMULATED AR HUD - STATUS NOMINAL")
        color: "#9aa4b2"
        font.pixelSize: 11
    }
}
