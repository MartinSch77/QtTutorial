// SPDX-License-Identifier: MIT
import QtQuick

// Radar-plot-style sensor track display - a passive visualization of
// simulated tracked objects (position, heading, a generic classification
// label). There is no weapons data, no aim/intercept solution and no
// launch/fire control anywhere in this panel: it is exactly the kind of
// screen an air-defence or maritime sensor operator watches, showing what
// is out there, not how to engage it.
Rectangle {
    id: root
    property var sim
    color: "#0a0f14"

    Canvas {
        id: canvas
        anchors.fill: parent
        anchors.margins: 16

        readonly property double areaRadiusKm: root.sim ? root.sim.trackAreaRadiusKm : 40.0

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
            const radius = Math.min(width, height) / 2 - 8;
            const scale = radius / canvas.areaRadiusKm;

            ctx.strokeStyle = "#1f2a33";
            ctx.lineWidth = 1;
            for (let ring = 1; ring <= 4; ++ring) {
                ctx.beginPath();
                ctx.arc(cx, cy, radius * ring / 4, 0, 2 * Math.PI);
                ctx.stroke();
            }
            ctx.beginPath();
            ctx.moveTo(cx - radius, cy);
            ctx.lineTo(cx + radius, cy);
            ctx.moveTo(cx, cy - radius);
            ctx.lineTo(cx, cy + radius);
            ctx.stroke();

            if (!root.sim) {
                return;
            }

            const tracks = root.sim.tracks;
            for (let i = 0; i < tracks.length; ++i) {
                const track = tracks[i];
                const px = cx + track.xKm * scale;
                const py = cy + track.yKm * scale;
                const headingRad = track.headingDeg * Math.PI / 180;

                ctx.fillStyle = "#3ddc6f";
                ctx.beginPath();
                ctx.arc(px, py, 5, 0, 2 * Math.PI);
                ctx.fill();

                ctx.strokeStyle = "#3ddc6f";
                ctx.lineWidth = 2;
                ctx.beginPath();
                ctx.moveTo(px, py);
                ctx.lineTo(px + Math.cos(headingRad) * 16, py + Math.sin(headingRad) * 16);
                ctx.stroke();

                ctx.fillStyle = "#f2f4f8";
                ctx.font = "12px sans-serif";
                ctx.fillText(track.classification + " #" + track.id, px + 8, py - 8);
            }
        }
    }
}
