// SPDX-License-Identifier: MIT
import QtQuick

// Helm/Navigation station: heading, warp/impulse speed readouts and a
// simple 2D top-down "local space" plot showing the ship at the center and
// a few simulated nearby objects drifting plausibly around it. Reflows from
// side-by-side (readouts | plot) on wide layouts to stacked (readouts above
// plot) on narrow ones, so the plot never has to squeeze into a negative
// width on a phone-sized window.
Item {
    id: root
    property var bridge
    readonly property bool wide: width >= 560

    Column {
        id: stats
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 16
        width: root.wide ? 220 : parent.width - 32
        spacing: 14

        Text { text: qsTr("HELM / NAVIGATION"); color: "#ff9c00"; font.pixelSize: 18; font.bold: true }

        Row {
            spacing: 24

            Column {
                spacing: 2
                Text { text: qsTr("HEADING"); color: "#9c8fae"; font.pixelSize: 12 }
                Text {
                    text: root.bridge ? root.bridge.headingDeg.toFixed(0) + "°" : "0°"
                    color: "#c6a0ff"
                    font.pixelSize: 24
                    font.bold: true
                }
            }
            Column {
                spacing: 2
                Text { text: qsTr("WARP"); color: "#9c8fae"; font.pixelSize: 12 }
                Text {
                    text: root.bridge ? root.bridge.warpFactor.toFixed(1) : "0.0"
                    color: "#c6a0ff"
                    font.pixelSize: 24
                    font.bold: true
                }
            }
            Column {
                spacing: 2
                Text { text: qsTr("IMPULSE"); color: "#9c8fae"; font.pixelSize: 12 }
                Text {
                    text: root.bridge ? root.bridge.impulsePercent.toFixed(0) + " %" : "0 %"
                    color: "#c6a0ff"
                    font.pixelSize: 24
                    font.bold: true
                }
            }
        }
    }

    Rectangle {
        id: plot
        anchors.left: root.wide ? stats.right : parent.left
        anchors.right: parent.right
        anchors.top: root.wide ? parent.top : stats.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: root.wide ? 24 : 16
        anchors.rightMargin: 16
        anchors.topMargin: 16
        anchors.bottomMargin: 16
        radius: 8
        color: "#0a0710"
        border.color: "#3a2f47"

        Canvas {
            id: canvas
            anchors.fill: parent
            anchors.margins: 12

            readonly property double areaRadiusKm: root.bridge ? root.bridge.navAreaRadiusKm : 50.0

            Timer {
                interval: 100
                running: true
                repeat: true
                onTriggered: canvas.requestPaint()
            }

            onPaint: {
                const ctx = getContext("2d");
                ctx.reset();
                const cx = width / 2;
                const cy = height / 2;
                const radius = Math.max(Math.min(width, height) / 2 - 8, 1);
                const scale = radius / canvas.areaRadiusKm;

                ctx.strokeStyle = "#241c2e";
                ctx.lineWidth = 1;
                for (let ring = 1; ring <= 4; ++ring) {
                    ctx.beginPath();
                    ctx.arc(cx, cy, radius * ring / 4, 0, 2 * Math.PI);
                    ctx.stroke();
                }

                ctx.fillStyle = "#ff9c00";
                ctx.beginPath();
                ctx.arc(cx, cy, 6, 0, 2 * Math.PI);
                ctx.fill();
                ctx.fillStyle = "#f2e9ff";
                ctx.font = "11px sans-serif";
                ctx.fillText(qsTr("OWN SHIP"), cx + 10, cy + 4);

                if (!root.bridge) {
                    return;
                }

                const contacts = root.bridge.navContacts;
                for (let i = 0; i < contacts.length; ++i) {
                    const contact = contacts[i];
                    const px = cx + contact.xKm * scale;
                    const py = cy + contact.yKm * scale;

                    ctx.fillStyle = "#c6a0ff";
                    ctx.beginPath();
                    ctx.arc(px, py, 4, 0, 2 * Math.PI);
                    ctx.fill();

                    ctx.fillStyle = "#c6a0ff";
                    ctx.font = "10px sans-serif";
                    ctx.fillText(contact.label, px + 7, py - 6);
                }
            }
        }
    }
}
