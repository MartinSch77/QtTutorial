// SPDX-License-Identifier: MIT
import QtQuick

import "Icons.js" as Icons

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
                const cardinals = {0: "N", 90: "E", 180: "S", 270: "W"};
                const heading = root.sim ? root.sim.hudHeadingDeg : 0;
                for (let d = -60; d <= 60; d += 15) {
                    const deg = Math.round(((heading + d) % 360 + 360) % 360);
                    const px = width / 2 + d * (width / 130);
                    ctx.beginPath();
                    ctx.moveTo(px, height * 0.6);
                    ctx.lineTo(px, height);
                    ctx.stroke();
                    if (deg % 90 === 0 && cardinals[deg] !== undefined) {
                        ctx.save();
                        ctx.fillStyle = "#ffe27a";
                        ctx.font = "bold 13px sans-serif";
                        ctx.fillText(cardinals[deg], px - 4, height * 0.42);
                        ctx.restore();
                    } else {
                        ctx.fillText(deg + "°", px - 10, height * 0.45);
                    }
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

    Canvas {
        id: statusOverlay
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 20
        width: 90
        height: 60

        readonly property bool anySubsystemCritical: {
            const list = root.sim ? root.sim.subsystems : [];
            for (let i = 0; i < list.length; ++i) {
                if (list[i].state === "Critical") return true;
            }
            return false;
        }
        readonly property double commsQuality: root.sim ? root.sim.commsQualityPercent : 100

        onPaint: {
            const ctx = getContext("2d");
            ctx.reset();
            Icons.drawAntennaBars(ctx, 16, 16, 12, commsQuality, "#9adfff");
            ctx.fillStyle = "#9adfff";
            ctx.font = "10px sans-serif";
            ctx.fillText(Math.round(commsQuality) + "%", 32, 20);
            if (anySubsystemCritical) {
                Icons.drawWarningTriangle(ctx, 20, 44, 10, "#e0a300");
                ctx.fillStyle = "#e0a300";
                ctx.font = "bold 10px sans-serif";
                ctx.fillText(qsTr("SUBSYSTEM"), 34, 42);
                ctx.fillText(qsTr("CRITICAL"), 34, 54);
            }
        }

        Timer {
            interval: 200
            running: true
            repeat: true
            onTriggered: statusOverlay.requestPaint()
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
